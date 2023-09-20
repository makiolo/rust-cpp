#pragma once

#include "serie.h"

namespace rp
{
    std::shared_ptr<Serie> diff(const std::shared_ptr<Serie> &s0, int period = 1);

    Serie diff(const Serie& s0, int period = 1);
}
