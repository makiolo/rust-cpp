#pragma once

#include "serie.h"

namespace rp
{
    std::shared_ptr<Serie> shift(const std::shared_ptr<Serie> &s0, int period = 1);

    Serie shift(const Serie& s0, int period = 1);
}
