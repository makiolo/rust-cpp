#pragma once

#include "serie.h"

namespace rp
{
    std::shared_ptr<Serie> sqrt(const std::shared_ptr<Serie> &s0);

    Serie sqrtref(const Serie& s0);
}
