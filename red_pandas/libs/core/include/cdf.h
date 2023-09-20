#pragma once

#include "serie.h"

namespace rp {
    std::shared_ptr<Serie> cdf(const std::shared_ptr<Serie>& s0);

    Serie cdf(const Serie& s0);
}
