#pragma once

#include "serie.h"

namespace rp {
    bool any_isfinite(const std::shared_ptr<Serie> &s0);
    bool all_isfinite(const std::shared_ptr<Serie> &s0);

    bool any_isfinite(const Serie& s0);
    bool all_isfinite(const Serie& s0);
}
