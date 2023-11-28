import math
import numpy as np
import pandas as pd
import red_pandas as rp


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


if __name__ == '__main__':

    import pytest
    args_str = "-rP -k test_volatility"
    args = args_str.split(" ")
    pytest.main(args)
