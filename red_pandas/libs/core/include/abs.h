#pragma once

#include "serie.h"

namespace rp {
    std::shared_ptr<Serie> abs(const std::shared_ptr<Serie> &s0);

    Serie abs(const Serie& s0);
}
