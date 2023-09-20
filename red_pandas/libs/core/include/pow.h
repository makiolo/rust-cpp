#pragma once

#include "serie.h"

namespace rp {
    std::shared_ptr<Serie> pow(const std::shared_ptr<Serie> &s0, const std::shared_ptr<Serie> &s1);

    Serie powref(const Serie& s0, const Serie& s1);
}
