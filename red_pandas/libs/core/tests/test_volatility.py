import sys
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

def test_matplotlib():
    paths = 200
    steps = 100
    current_price = 100.0
    r = rp.constant(0.05)
    q = rp.constant(0.02)
    sigma = rp.constant(0.31)
    dcf = rp.constant(1.0 / 365.0)
    T = dcf * rp.constant(steps)
    dt = T / rp.constant(steps)

    x = []
    S = rp.array([current_price] * paths)
    for i in range(int(steps)):
        S = model_brownian_gbm(S, r, q, sigma, dt)
        x.append(S.to_numpy())
    plt.plot(range(int(steps)), x)
    plt.show()


if __name__ == '__main__':

    args_str = "-rP -k test_"
    args = args_str.split(" ")
    sys.exit(pytest.main(args))
