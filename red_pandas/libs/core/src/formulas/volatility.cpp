#include "formulas/volatility.h"
#include "formulas/greeks.h"
#include "sqrt.h"
#include "log.h"
#include "pow.h"

namespace rp {

    // https://www.mlfinlab.com/en/latest/feature_engineering/volatility_estimators.html
    rp::column_ptr parkinson(   int period,
                                const rp::column_ptr& high,
                                const rp::column_ptr& low)
    {
        auto d = rp::array({4.0 * period}) * rp::log2();
        auto t = rp::pow(rp::log(high / low), rp::two());
        auto s = rp::sum({rp::calculate(t)});
        return rp::calculate(rp::sqrt(s / d));
    }
}
