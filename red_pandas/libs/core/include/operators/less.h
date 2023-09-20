#pragma once

#include "serie.h"

namespace rp {
    bool any_less(const std::shared_ptr<Serie> &s0, const std::shared_ptr<Serie> &s1);
    bool all_less(const std::shared_ptr<Serie> &s0, const std::shared_ptr<Serie> &s1);

    bool any_less(const Serie& s0, const Serie& s1);
    bool all_less(const Serie& s0, const Serie& s1);
}
