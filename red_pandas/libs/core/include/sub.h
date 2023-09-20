#pragma once

#include "serie.h"

namespace rp {
    // Python
    std::shared_ptr<Serie> sub2(const std::shared_ptr<Serie> &s0, const std::shared_ptr<Serie> &s1);

    // C++
    Serie sub2ref(const Serie& s0, const Serie& s1);
}
