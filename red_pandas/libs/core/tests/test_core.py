# -*- coding: UTF-8 -*-

import sys
import json
import time
import numpy as np
import red_pandas as rp
from _pytest.python_api import approx
from scipy.stats import norm


def zc2df(r, years, compound_times, npv=True):
    if npv:
        begin_step=1
        offset_step=0
    else:
        begin_step=0
        offset_step=-1
    print('Expanding since python ...')
    begin = time.time()
    day_counts = [float(x) for x in range(begin_step, years * compound_times + offset_step)]
    n = rp.Serie(np.array(day_counts, dtype='float64'))
    r = rp.Serie(np.array([r] * len(day_counts), dtype='float64'))
    one = rp.Serie(np.array([1.0] * len(day_counts), dtype='float64'))
    b = rp.Serie(np.array([float(compound_times)] * len(day_counts), dtype='float64'))
    print('end expand.')
    elapsed = time.time() - begin
    print('end expand, time: {}.'.format(elapsed))

    begin = time.time()
    dfs = (one / rp.pow(one + (r / b), n * b)) + (one / rp.pow(one + (r / b), n * b)) + (one / rp.pow(one + (r / b), n * b)) + (one / rp.pow(one + (r / b), n * b))
    dfs.wait()
    elapsed = time.time() - begin
    return dfs, elapsed


def zc2df_numpy(r, years, compound_times, npv=True):
    if npv:
        begin_step=1
        offset_step=0
    else:
        begin_step=0
        offset_step=-1
    print('Expanding since python ...')
    begin = time.time()
    day_counts = [float(x) for x in range(begin_step, years * compound_times + offset_step)]
    n = np.array(day_counts)
    r = np.array([r] * len(day_counts))
    one = np.array([1.0] * len(day_counts))
    b = np.array([float(compound_times)] * len(day_counts))
    elapsed = time.time() - begin
    print('end expand, time: {}.'.format(elapsed))

    begin = time.time()
    dfs = (one / np.power(one + (r / b), n * b)) + (one / np.power(one + (r / b), n * b)) + (one / np.power(one + (r / b), n * b)) + (one / np.power(one + (r / b), n * b))
    # print(dfs)
    elapsed = time.time() - begin
    return dfs, elapsed


def ImpliedVolatility(S, K, r, q, Vol_guess, T, market_price, flag='c', tol=0.00001, lr=1.0, max_iter=2000):
    PRECISION = rp.Serie(tol)
    lr = rp.Serie(lr)
    sigma = Vol_guess
    for _ in range(max_iter):
        d_uno = d1(S, K, r, q, T, sigma)
        if flag == 'c':
            price = CallPrice(S, K, r, q, T, Vol=sigma, d_uno=d_uno)
        else:
            price = PutPrice(S, K, r, q, T, Vol=sigma, d_uno=d_uno)
        diff = market_price - price
        if rp.all_less(rp.abs(diff), PRECISION):
            # OK
            return sigma
        Vega = S * rp.sqrt(T) * rp.pdf(d_uno)
        sigma = sigma + (diff / Vega) * lr
    return rp.abs(sigma)




def d1_numpy(S, K, r, q, T, Vol):
    assert(Vol is not None)
    return (np.log(S / K) + (r - q + np.power(Vol, 2.0) / 2.0) * T) / Vol * np.sqrt(T)

def d2_numpy(d_uno, T, Vol):
    return d_uno - (Vol * np.sqrt(T))

def CallPrice_numpy(S, K, r, q, T, Vol=None, d_uno=None, d_dos=None):
    if d_uno is None:
        d_uno = d1_numpy(S, K, r, q, T, Vol)
    if d_dos is None:
        # d_dos = d2(S, K, r, q, T, Vol)
        d_dos = d2_numpy(d_uno, T, Vol)
    return (S * np.exp(-(q * T)) * norm.cdf(d_uno)) - (K * np.exp(-(r * T)) * norm.cdf(d_dos))




def d1(S, K, r, q, T, Vol):
    assert(Vol is not None)
    return (rp.log(S / K) + (r - q + rp.pow(Vol, rp.two()) / rp.two()) * T) / Vol * rp.sqrt(T)

def d2(d_uno, T, Vol):
    return d_uno - (Vol * rp.sqrt(T))

# def d2(S, K, r, q, T, Vol):
#     assert(Vol is not None)
#     return (rp.log(S / K) + (r - q - rp.pow(Vol, rp.two()) / rp.two()) * T) / Vol * rp.sqrt(T)

def CallPrice(S, K, r, q, T, Vol=None, d_uno=None, d_dos=None):
    if d_uno is None:
        d_uno = d1(S, K, r, q, T, Vol)
    if d_dos is None:
        # d_dos = d2(S, K, r, q, T, Vol)
        d_dos = d2(d_uno, T, Vol)
    return (S * rp.exp(-(q * T)) * rp.cdf(d_uno)) - (K * rp.exp(-(r * T)) * rp.cdf(d_dos))

def PutPrice(S, K, r, q, T, Vol=None, d_uno=None, d_dos=None):
    if d_uno is None:
        d_uno = d1(S, K, r, q, T, Vol)
    if d_dos is None:
        # d_dos = d2(S, K, r, q, T, Vol)
        d_dos = d2(d_uno, T, Vol)
    return (K * rp.exp(-(r * T)) * rp.cdf(-d_dos)) - (S * rp.exp(-(q * T)) * rp.cdf(-d_uno))

def CallGreeks(S, K, r, T, Vol, d_uno, d_dos):
    Delta = rp.cdf(d_uno)
    Gamma = rp.pdf(d_uno) / (S * Vol * rp.sqrt(T))
    Theta = (-(S * Vol * rp.pdf(d_uno)) / (rp.two() * rp.sqrt(T))) - (r*K*rp.exp(-(r*T)) * rp.cdf(d_dos))
    Vega = S * rp.sqrt(T) * rp.pdf(d_uno)
    Rho = K * T * rp.exp(-(r*T)) * rp.cdf(d_dos)
    return Delta, Gamma, Theta, Vega, Rho

def PutGreeks(S, K, r, T, Vol, d_uno, d_dos):
    Delta = rp.cdf(d_uno) - rp.one()
    Gamma = rp.pdf(d_uno) / (S * Vol * rp.sqrt(T))
    Theta = (-(S * Vol * rp.pdf(d_uno)) / (rp.two() * rp.sqrt(T))) + (r*K*rp.exp(-(r*T)) * rp.cdf(-d_dos))
    Vega = S * rp.sqrt(T) * rp.pdf(d_uno)
    Rho = K * T * rp.exp(-(r*T)) * rp.cdf(-d_dos)
    return Delta, Gamma, Theta, Vega, Rho


def test_call_put():
    # stock price
    S = 48.40
    # current volatility
    Vol = 0.18
    # current interest rate (sensibility here)
    r = 0.0
    # current dividend (sensibility here)
    q = 0.0

    '''
    # https://es.investing.com/equities/apple-computer-inc-options
    market = {
        'options': {
            'AAPL': {
                '1M': {
                    'strikes': [70, 75, 80, 149, 150, 152.5],
                    'call': {
                        'buy': [78.85, 73.8, 68.9, 5.6, 5.05, 3.9],
                        'sell': [0.01, 0.02, 0.02, 5.5, 6, 7.3],
                    },
                    'put': {
                        'buy': [78.85, 73.8, 68.9, 5.6, 5.05, 3.9],
                        'sell': [0.01, 0.02, 0.02, 5.5, 6, 7.3],
                    }
                },
                '2M': {
                    'strikes': [70, 75, 80, 149, 150, 152.5],
                    'call': {
                        'buy': [78.85, 73.8, 68.9, 5.6, 5.05, 3.9],
                        'sell': [0.01, 0.02, 0.02, 5.5, 6, 7.3],
                    },
                    'put': {
                        'buy': [78.85, 73.8, 68.9, 5.6, 5.05, 3.9],
                        'sell': [0.01, 0.02, 0.02, 5.5, 6, 7.3],
                    }
                },
            }
        }
    }
    '''

    # ASSET x TENOR X STRIKES
    # 500 assets x 24 tenors x 200 strikes = 2.400.000

    # call_strike_range = [70, 75, 80, 149, 150, 152.5]
    # put_strike_range = [70, 75, 80, 149, 150, 152.5]
    # call_price_range = [78.85, 73.8, 68.9, 5.6, 5.05, 3.9]
    # put_price_range = [0.01, 0.02, 0.02, 5.5, 6, 7.3]

    # time to expiration
    maturity_range = [ 56 / 252.0,  56 / 252.0,  56 / 252.0,  56 / 252.0,  56 / 252.0,  56 / 252.0,
                      112 / 252.0, 112 / 252.0, 112 / 252.0, 112 / 252.0, 112 / 252.0, 112 / 252.0]
    # Exercise price
    call_strike_range = [44, 46, 48, 50, 52, 54,
                         44, 46, 48, 50, 52, 54]
    put_strike_range = [44, 46, 48, 50, 52, 54,
                        44, 46, 48, 50, 52, 54]

    # option call price (market)
    call_price_range = [4.59, 2.99, 1.75, 0.93, 0.47, 0.23,
                        4.96, 3.52, 2.38, 1.55, 0.97, 0.60]

    # option put price (market)
    put_price_range = [0.13, 0.46, 1.16, 2.33, 3.88, 5.69,
                       0.42, 0.92, 1.72, 2.86, 4.29, 5.95]

    #######
    Np = []
    Sp = []
    KCp = []
    KPp = []
    Rp = []
    qp = []
    vp = []
    tp = []
    Cp = []
    Pp = []
    for _ in range(1000):
        for i in range(len(call_strike_range)):
            kc = call_strike_range[i]
            kp = put_strike_range[i]
            t = maturity_range[i]
            c = call_price_range[i]
            p = put_price_range[i]
            Sp.append(S)
            vp.append(Vol)
            Rp.append(r)
            qp.append(q)
            KCp.append(kc)
            KPp.append(kp)
            tp.append(t)
            Cp.append(c)
            Pp.append(p)

    # for i in range(len(Sp)):
    # for i in [230, ]:
    #     paths = geo_paths(Sp[i], Rp[i], qp[i], vp[i], tp[i], paths=10, steps=3)
    #     call_option_price = np.mean(np.maximum(paths[-1] - Kp[i], 0)) * np.exp(-Rp[i] * tp[i])
    #     put_option_price = -np.mean(np.minimum(paths[-1] - Kp[i], 0)) * np.exp(-Rp[i] * tp[i])
    #     print('{} - montecarlo call: {}'.format(i, call_option_price))
    #     print('{} - montecarlo put: {}'.format(i, put_option_price))

    print('calculating {} elements X10 greeks.'.format(len(Sp)))
    begin = time.time()
    S = rp.Serie(np.array(Sp, dtype='float64'))
    Kc = rp.Serie(np.array(KCp, dtype='float64'))
    Kp = rp.Serie(np.array(KPp, dtype='float64'))
    r = rp.Serie(np.array(Rp, dtype='float64'))
    q = rp.Serie(np.array(qp, dtype='float64'))
    Vol = rp.Serie(np.array(vp, dtype='float64'))
    T = rp.Serie(np.array(tp, dtype='float64'))
    C = rp.Serie(np.array(Cp, dtype='float64'))
    P = rp.Serie(np.array(Pp, dtype='float64'))

    print('S: {}'.format(S))
    print('Kc: {}'.format(Kc))
    print('Kp: {}'.format(Kp))
    print('r: {}'.format(r))
    print('q: {}'.format(q))
    print('Vol: {}'.format(Vol))
    print('T: {}'.format(T))
    print('C: {}'.format(C))
    print('P: {}'.format(P))

    # requirements
    print('-- Call: --')

    d_uno = d1(S, Kc, r, q, T, Vol)
    d_dos = d2(d_uno, T, Vol)

    '''
    SN = np.array(Sp, dtype='float64')
    KcN = np.array(KCp, dtype='float64')
    KpN = np.array(KPp, dtype='float64')
    rN = np.array(Rp, dtype='float64')
    qN = np.array(qp, dtype='float64')
    VolN = np.array(vp, dtype='float64')
    TN = np.array(tp, dtype='float64')
    CN = np.array(Cp, dtype='float64')
    PN = np.array(Pp, dtype='float64')
    '''

    # d_uno_numpy = d1_numpy(SN, KcN, rN, qN, TN, VolN)
    # d_dos_numpy = d2_numpy(d_uno_numpy, TN, VolN)
    #
    # N = 100
    #
    # begin = time.time()
    # for _ in range(N):
    #     call_price_hist_c = CallPrice_numpy(SN, KcN, rN, qN, TN, VolN, d_uno=d_uno_numpy, d_dos=d_dos_numpy)
    # elpased_numpy = time.time() - begin
    #
    # begin = time.time()
    # for _ in range(N):
    #     call_price_hist = CallPrice(S, Kc, r, q, T, Vol, d_uno=d_uno, d_dos=d_dos)
    #     call_price_hist.wait()
    # elpased_python = time.time() - begin
    #
    #
    # begin = time.time()
    # for _ in range(N):
    #     call_price_hist_b = rp.CallPrice(S, Kc, r, q, T, Vol, d_uno=d_uno, d_dos=d_dos)
    #     call_price_hist_b.wait()
    # elpased_cpp = time.time() - begin
    #
    # print('elapsed numpy: {}'.format(elpased_numpy))
    # print('elapsed python: {}'.format(elpased_python))
    # print('elapsed cpp: {}'.format(elpased_cpp))

    call_price_hist = CallPrice(S, Kc, r, q, T, Vol, d_uno=d_uno, d_dos=d_dos)

    print('Call Asset: {}'.format(C))
    print('Call Theorical Asset: {}'.format(call_price_hist))
    # print('Call Theorical Asset C++: {}'.format(call_price_hist_b))
    # print('Call Theorical Asset Numpy: {}'.format(call_price_hist_c))
    #
    # # call

    # new requirements
    # d_uno = d1(S, Kc, r, q, T, Vol)
    # d_dos = d2(S, Kc, r, q, T, Vol)
    #
    # call_price = CallPrice(S, Kc, r, q, T, d_uno=d_uno, d_dos=d_dos)
    CDelta, CGamma, CTheta, CVega , CRho = CallGreeks(S, Kc, r, T, Vol, d_uno, d_dos)

    # sensibilidad al precio (orden 1)
    print('Call Delta: {}'.format(CDelta))

    # sensibilidad al precio (orden 2)
    print('Call Gamma: {}'.format(CGamma))

    # sensibilidad al tiempo. Cuanto baja el valor de la opción cada día.
    print('Call Theta: {}'.format(CTheta))

    # sensibilidad a una subida del 1% de la volatilidad implicita
    print('Call Vega: {}'.format(CVega))

    # sensibilidad a una subida del 1% de los tipos de interes
    print('Call Rho: {}'.format(CRho))

    Call_Vol_implied = ImpliedVolatility(S, Kc, r, q, Vol, T, C, flag='c', tol=0.0001, lr=0.33, max_iter=2000)
    print('Call Vol Implied: {}'.format(Call_Vol_implied))

    print('-- Put: --')
    # requirements
    d_uno = d1(S, Kp, r, q, T, Vol)
    # d_dos = d2(S, Kp, r, q, T, Vol)
    d_dos = d2(d_uno, T, Vol)
    put_price_hist = PutPrice(S, Kp, r, q, T, d_uno=d_uno, d_dos=d_dos)
    print('Put Asset: {}'.format(P))
    print('Put Theorical Asset: {}'.format(put_price_hist))
    #

    # new requirements
    # d_dos = d2(d_uno, T, Put_Vol_implied)
    # put_price = PutPrice(S, Kp, r, q, T, d_uno=d_uno, d_dos=d_dos)

    PDelta, PGamma, PTheta, PVega , PRho = PutGreeks(S, Kp, r, T, Vol, d_uno, d_dos)

    print('Put Delta: {}'.format(PDelta))
    print('Put Gamma: {}'.format(PGamma))
    print('Put Theta: {}'.format(PTheta))
    print('Put Vega: {}'.format(PVega))
    print('Put Rho: {}'.format(PRho))
    # # put
    Put_Vol_implied = ImpliedVolatility(S, Kp, r, q, Vol, T, P, flag='p', tol=0.0001, lr=0.33, max_iter=2000)
    print('Put Vol implied: {}'.format(Put_Vol_implied))

    # comparacion
    # s1 < s2 ----> vector<bool>
    # s1 > s2 ----> vector<bool>
    # s1 <= s2 ----> vector<bool>
    # s1 >= s2 ----> vector<bool>
    # s1 == s2 ----> vector<bool>
    # s1 != s2 ----> vector<bool>
    # agregadores numeros
    # rp.mean(vector<double>) -> double
    # rp.std(vector<double>) -> double
    # rp.pct_change(vector<double>) -> double
    # rp.cumprod()
    # rp.cumsum()
    # agregadores booleanos
    # rp.any(vector<bool>) -> bool
    # rp.all(vector<bool>) -> bool

    # fig, ax = plt.subplots()
    # ax.spines['bottom'].set_position('zero')
    # ax.plot(tp,vp,label='vp',color='r')
    # plt.xlabel('Stock Asset')
    # plt.ylabel('Profit and loss')
    # plt.legend()
    # plt.show()

    elapsed = time.time() - begin
    print('Elapsed time: {}'.format(elapsed))


def test_bench_zero_coupon():
    print('--- TEST 1: Compute IL with cache')

    # years = 10000000
    years = 100
    rate_interest = 0.16
    compound_times = 4

    print('Calculating red_pandas ...')
    dfs, elapsed = zc2df(rate_interest, years, compound_times)
    # print(dfs)
    print('elapsed = {}'.format(elapsed))

    print('Calculating numpy ...')
    dfs, elapsed = zc2df_numpy(rate_interest, years, compound_times)
    # print(dfs)
    print('elapsed = {}'.format(elapsed))

    '''
    lista1 = [9.1, 2.1, 2.1, 2.1, 2.1, 2.1, 2.1]
    lista2 = [3.1, 2.1, 2.1, 2.1, 2.1, 2.1, 4.1]

    s0 = parse_il(lista1)
    s1 = parse_il(lista2)
   
    begin = time.time()
    '''

    # print(s0 + s1)
    # print(s0 * s1)
    #
    # print((s1 * s1) - (s1 * s0))

    # resultA = a.get()
    # resultB = b.get()
    # resultC = c.get()
    # resultD = d.get()
    # elapsed = time.time() - begin

    # print(f'sum2:\n{pprint_il(resultA)}')
    # print(f'mul2:\n{pprint_il(resultB)}')
    # print(f'div2:\n{pprint_il(resultC)}')
    # print(f'sub2:\n{pprint_il(resultD)}')
    # print(f'first elapsed {elapsed}')


def test_read_write_il():

    root = rp.read_il(r"Test07_20160421_154847_Input.il")

    amortizing = root['INPUT_PARAMS']['AMORTIZING_DATES']
    amortizing.set(amortizing * rp.sqrt(rp.constant(2.0)))

    root['MCY_SCRIPT'].set("Test write basic string")

    # read_hdf5
    # write_hdf5
    rp.write_il(root, r"Test07_20160421_154847_Input_MODIFIED.il")
    rp.write_json(root, r"Test07_20160421_154847_Input_MODIFIED.json", indent=True)


def test_basic_usage():

    def Formula(S, K, T):
        return rp.log(S) * K - T

    data = {
        'S': [7,7,7,7,7],
        'K': [1,2,3,4,5],
        'T': [5,5,5,5,5]
    }
    # convert from python
    s1 = rp.loads(data)

    for key, value in s1.iteritems():
        print('key: {}, value: {}'.format(key, repr(value)))
        for x in value:
            print('\t\t- {}'.format(x))

    calculation = Formula(s1['S'], s1['K'], s1['T'])
    print('processing ...')
    print(calculation)

    # convert to python
    print(rp.dumps(calculation))


def test_read_json():
    serie = rp.read_json(r"example.json")
    serie['glossary']['GlossDiv']['GlossList']['GlossEntry']['GlossDef']['GlossSeeAlso'].set( ['GML', 'XML', 'Ricardo'] )
    rp.write_json(serie, r"example_MODIFIED.json", indent=True)


def test_shape():
    print( np.shape( rp.dumps( rp.loads([[[1,2,3],[4,5,6]], [[1,2,3],[4,5,6]], [[1,2,3],[4,5,6]]]), recursive=True ) ) )
    print( rp.array([1,2,3]) )
    print( rp.array([1.0, 2.0, 3.0]) )
    print( rp.array([1,2,3]) )
    print( rp.array([1.0, 2.0, 3.0]) )


def test_callback():

    def py_callback(i, s):
        print('py_callback(%d, %s)'%(i, s))

    rp.use_callback(py_callback)
    rp.use_callback(lambda x,y: py_callback(2*x, 'py_callback_lambda(%d, %s)' % (x,y)))


def test_schedule():

    res = rp.on_capital(1000, 5000, rp.Maturity(5))
    print(res.value)

    r = 0.08

    term1 = rp.TermStructure()
    term1.append_spot(rp.Maturity(1), rp.InterestRate(r, rp.YIELD, rp.ANNUAL))
    term1.append_spot(rp.Maturity(2), rp.InterestRate(r, rp.YIELD, rp.ANNUAL))
    term1.append_spot(rp.Maturity(3), rp.InterestRate(r, rp.YIELD, rp.ANNUAL))
    term1.build()

    # create CashFlows free (passing TermStructure)
    coupon = rp.CouponCashFlow(term1, 9.99 * 12)
    assert(coupon.to_start_cashflow().cash == approx(308.9423868313))
    assert(coupon.to_end_cashflow().cash == approx(389.178432))

    # create CashFlows from TermStructure (good idea ?)
    initial = term1.make_cashflow(rp.Maturity(0.0), 1000.0)
    final = term1.make_cashflow(rp.Maturity(3.0), 1259.712)
    other = term1.make_cashflow(rp.Maturity(2.0), 3059.712)

    print(initial.to_custom_cashflow(rp.Maturity(1)).cash)
    print(final.to_custom_cashflow(rp.Maturity(2)).cash)

    # create Leg Products and value
    leg1 = rp.Leg()
    leg1.add(initial)
    leg1.add(final)
    print('leg1 npv = {}'.format(leg1.npv()))

    leg2 = rp.Leg()
    leg2.add(other)
    print('leg2 npv = {}'.format(leg2.npv()))

    product = rp.Product()
    product.add(leg1)
    product.add(leg2)
    print('product npv = {}'.format(product.npv()))


def test_blockchain():
    blockchain = rp.BlockChain()
    blockchain.add_transaction(0, 1, 1000.0, "EUR")
    blockchain.add_transaction(1, 2, 1000.0, "EUR")
    blockchain.add_transaction(0, 1, 1000.0, "EUR")
    blockchain.add_transaction(1, 2, 1000.0, "EUR")
    blockchain.add_transaction(0, 1, 1000.0, "EUR")
    blockchain.add_transaction(1, 2, 1000.0, "EUR")
    blockchain.add_transaction(0, 1, 1000.0, "EUR")
    blockchain.add_transaction(1, 2, 1000.0, "EUR")
    blockchain.add_transaction(2, 1, 500.0, "EUR")
    blockchain.add_transaction(0, 1, 3.0, "Vacas")

    for wallet in [1, 2, 3]:
        bal = blockchain.balance(wallet)
        for k, v in bal.iteritems():
            print(wallet, k, v)


def test_window():

    # initial price
    S0 = rp.constant(48.40)
    # anual volatility
    Vol = rp.constant(0.18)
    # performance expected
    r_exp = rp.InterestRate(0.08, rp.YIELD).to_other_interest_rate(rp.EXPONENTIAL).value
    r = rp.constant(r_exp)

    # dividends
    q = rp.constant(0.03)
    # valuation date of simulation
    valuation_date = '2023-05-14'
    # start date deals
    start_deals = ['2023-04-14', '2023-01-15']
    # end date deals
    maturities = ['2023-07-14', '2023-12-15']
    # type deal
    type_deals = ['MCEuropeanCall', 'MCAmericanCall']
    # strikes (best use porcentages)
    K = rp.array([43.0, 48.0, 53.0, 58.0])
    # maturities in day count
    maturities_day_count = [3.0, 365.0]
    # paths to use
    paths = int(1)
    # day count convention
    dcf = rp.constant(1.0 / 365.0)

    def model_brownian_gbm(S, r, q, sigma, dt):
        # TODO: saturday and sunday, use friday price
        # implement diffusion jumps
        Z = rp.rand_normal(S.size())
        return S * rp.exp(
            (r - q - rp.pow(sigma, rp.two()) / rp.two()) * dt +
            (Z * sigma * rp.sqrt(dt))
        )

    for m in maturities_day_count:

        steps = rp.constant(m)
        T = dcf * steps
        dt = T / steps
        to_present = rp.exp(-r * T)

        pos = []
        neg = []

        for _ in range(paths):

            S = S0

            # sequence must be here
            for i in range(int(m)):
                S = model_brownian_gbm(S, r, q, Vol, dt)

            # fuera del while = EUROPEAN ?
            # dentro del while = AMERICAN ?
            last_minus_K = rp.max0(S - K)
            K_minus_last = rp.max0(K - S)

            pos.append(last_minus_K)
            neg.append(K_minus_last)

        call_price = rp.mean_transpose(pos) * to_present
        put_price = rp.mean_transpose(neg) * to_present

        print('call_price mat {} montecarlo = {}'.format(m, call_price))
        print('put_price mat {} montecarlo = {}'.format(m, put_price))

        duno = rp.d1(S0, K, r, q, T, Vol)
        ddos = rp.d2(S0, K, r, q, T, Vol)

        call_price_analythic = rp.CallPrice(S0, K, r, q, T, Vol, duno, ddos)
        put_price_analythic = rp.PutPrice(S0, K, r, q, T, Vol, duno, ddos)

        print('call_price mat {} black scholes = {}'.format(m, call_price_analythic))
        print('put_price mat {} black scholes = {}'.format(m, put_price_analythic))

        print('-- Teorica mat {} media y varianza --'.format(m))

        r_theorical = (r - q - (rp.pow(Vol, rp.two()) / rp.two())) * T
        print("rate exp. = {:.2f}%".format(r_theorical.to_vector()[0] * 100.0))

        ir = rp.InterestRate(r_theorical.to_vector()[0], rp.EXPONENTIAL).to_other_interest_rate(rp.YIELD)
        print("rate yield = {:.2f}%".format(ir.value * 100.0))

        vol_theorical = Vol * rp.sqrt(T)
        print("volatility of period {} = {:.2f}%".format(T, vol_theorical.to_vector()[0] * 100.0))


def _test_serie_integer():
    n = rp.Serie(5.0, rp.DISCRETE_MUTABLE)
    print(n)


if __name__ == '__main__':

    import pytest
    args_str = "-rP -k test_core"
    args = args_str.split(" ")
    sys.exit(pytest.main(args))
