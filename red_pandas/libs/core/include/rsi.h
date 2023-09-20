#pragma once

#include "serie.h"

namespace rp
{
    std::shared_ptr<Serie> rsi(const std::shared_ptr<Serie> &s0, int period = 14);

    Serie rsi(const Serie& s0, int period = 14);
}
