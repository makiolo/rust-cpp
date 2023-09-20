#pragma once

#include "serie.h"

namespace rp {
    std::shared_ptr<Serie> clean(const std::shared_ptr<Serie> &s0, bool replace = false, double neutral = 0.0);

    Serie clean(const Serie& s0, bool replace = false, double neutral = 0.0);
}
