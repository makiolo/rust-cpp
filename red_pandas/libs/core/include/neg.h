#pragma once

#include "serie.h"

namespace rp {
    std::shared_ptr<Serie> neg(const std::shared_ptr<Serie> &s0);

    Serie neg(const Serie& s0);
}
