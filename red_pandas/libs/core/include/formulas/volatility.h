#ifndef RED_PANDAS_PROJECT_VOLATILITY_H
#define RED_PANDAS_PROJECT_VOLATILITY_H

#include "serie.h"
#include "sum.h"

namespace rp {

    rp::column_ptr parkinson(   int period,
                                const rp::column_ptr& high,
                                const rp::column_ptr& low);

}

#endif