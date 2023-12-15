import sys
import time
import random
from collections import defaultdict

import pytest
##
import math
import numpy as np
import red_pandas as rp
import matplotlib.pyplot as plt


def get_estimator(price_data, window=30, trading_periods=252, clean=True):

    rs = (1.0 / (4.0 * math.log(2.0))) * ((price_data['High'] / price_data['Low']).apply(np.log))**2.0

    def f(v):
        return (trading_periods * v.mean())**0.5

    result = rs.rolling(
        window=window,
        center=False
    ).apply(func=f)

    if clean:
        return result.dropna()
    else:
        return result


def test_volatility_parkinson():
    period = 30
    initial_price = 100
    elements = 5
    close = rp.constant(initial_price) + (rp.constant(12.0) * rp.rand_normal(elements))
    high = close + (rp.constant(3.0) * rp.rand_normal(elements))
    low = close - (rp.constant(3.0) * rp.rand_normal(elements))

    # df = pd.DataFrame({'High': high.to_vector(), 'Low': low.to_vector()})
    # print(df)

    result = rp.onehundred() * rp.parkinson(period, high, low)
    print(result.to_vector()[0])


def model_brownian_gbm(S, r, q, sigma, dt):
    Z = rp.rand_normal(S.size())
    return S * rp.exp(
        (r - q - rp.pow(sigma, rp.two()) / rp.two()) * dt +
        (Z * sigma * rp.sqrt(dt))
    )


def get_balance(blockchain, wallet, currency, fallback=0.0):
    balance_handler =blockchain.balance(wallet)
    for k, v in balance_handler.iteritems():
        if k == currency:
            return v
    return fallback


def test_volatility_matplotlib():
    begin = time.time()
    paths = 1
    steps = 1000
    current_price = 100.0
    current_balance = 10000.0
    current_bank_central = 1000000000
    r = rp.constant(0.05)
    q = rp.constant(0.02)
    sigma = rp.constant(0.31)
    dcf = rp.constant(1.0 / 365.0)
    T = dcf * rp.constant(steps)
    dt = T / rp.constant(steps)

    # Operaciones simultaneas segun el step:
    # 1000 = compra 1000
    # -1000 = venta 1000
    # 0 = sin cambios
    products = []
    # number operations to generate
    for _ in range(10):
        product = np.zeros(steps)
        size = int(random.uniform(10, 50))
        offset = int(random.uniform(size, steps - 2*size)) + 1
        for i in range(offset, offset + size):
            product[i] = 1000
        products.append(rp.array(product))

    # limitar maximo paralelismo de los productos
    # no mas de X productos ejecutandose simultaneamente

    signals_buys = []
    signals_sells = []
    for x in products:

        # convert NAN in 0.0
        dx = rp.clean(rp.diff(x), True, 0.0)

        buys = rp.max0(dx)
        signals_buys.append(buys)

        sells = rp.min0(dx)
        signals_sells.append(sells)

        # borramos los 0
        # si queda [1, -1] --> es una signal buy
        # si queda [-1, 1] --> es una signal sell

    signals_buys_transposed = rp.transpose(signals_buys)
    signals_sells_transposed = rp.transpose(signals_sells)

    blockchains = defaultdict(rp.BlockChain)
    for p in range(paths):
        blockchains[p].add_transaction(0, 1, current_balance, 'EUR')
        blockchains[p].add_transaction(0, 2, current_bank_central, 'ASSET_BUY_IN_EUR')
        blockchains[p].add_transaction(0, 2, current_bank_central, 'EUR')

    simulation = []
    balance = []
    equity = []
    #
    total_eur = 0.0
    # total_price = 0.0
    #
    S = rp.array([current_price] * paths)
    for i in range(int(steps)):
        S = model_brownian_gbm(S, r, q, sigma, dt)

        # new open positions
        local_signals_buys = signals_buys_transposed[i]
        if not rp.all_less(rp.abs(local_signals_buys), rp.threshold()):
            for j, qty in enumerate(local_signals_buys.to_vector()):
                if abs(qty) > 0.0:
                    for p in range(paths):
                        # TODO: y si compramos el asset en moneda extranjera ?
                        blockchains[p].add_transaction(1, 2, qty, 'EUR')
                        blockchains[p].add_transaction(2, 1, qty / S.read(p), 'ASSET_BUY_IN_EUR')
                        # esta operacion abre -> el beneficio se compara con el precio de mercado
                        # beneficio abierta = Precio compra - Precio mercado

        # closed open positions
        local_signals_sells = signals_sells_transposed[i]
        if not rp.all_less(rp.abs(local_signals_sells), rp.threshold()):
            for j, qty in enumerate(local_signals_sells.to_vector()):
                if abs(qty) > 0.0:
                    for p in range(paths):
                        asset_balance = get_balance(blockchains[p], 1, "ASSET_BUY_IN_EUR")
                        if asset_balance > 0.0:
                            blockchains[p].add_transaction(1, 2, asset_balance, 'ASSET_BUY_IN_EUR')
                            blockchains[p].add_transaction(2, 1, asset_balance * S.read(p), 'EUR')
                            # esta operacion cierra, se acumula al beneficio de cerradas
                            # Beneficio cierre = Precio venta - precio compra

        slide_balance = []
        slide_equity = []
        for p in range(paths):
            open_balance = get_balance(blockchains[p], 1, "ASSET_BUY_IN_EUR")
            close_balance = get_balance(blockchains[p], 1, "EUR")
            # invest + closed
            # equity = depositos + beneficios de cerradas + beneficios de abiertas
            # balance = depositos + beneficios de cerradas
            invest_balance = -(get_balance(blockchains[p], 2, "EUR") - current_bank_central)
            # slide_balance.append(((total_price / total_eur) * open_balance) + close_balance)
            # convert Asset quantity in Eur value
            slide_balance.append(invest_balance)

            slide_equity.append((S.read(p) * open_balance))

        balance.append(rp.array(slide_balance))
        equity.append(rp.array(slide_equity))
        simulation.append(S)

    if paths > 1:
        rp.plot_transpose(equity, name='equity')
        rp.plot_transpose(balance, name='balance')
    elif paths == 1:
        rp.transpose(equity)[0].plot(name='equity')
        rp.transpose(balance)[0].plot(name='balance')
    else:
        raise Exception("Invalid number paths.")

    rp.show(legend=True)

    elapsed = time.time() - begin
    print('Eleapsed time: {}'.format(elapsed))


if __name__ == '__main__':

    args_str = "-rP -k test_volatility"
    args = args_str.split(" ")
    sys.exit(pytest.main(args))
