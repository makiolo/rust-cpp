#pragma once

#include "serie.h"

namespace rp
{
    std::shared_ptr<Serie> max0(const std::shared_ptr<Serie> &s0);
    Serie max0ref(const Serie& s0);

    std::shared_ptr<Serie> max2(const std::shared_ptr<Serie>& s0, const std::shared_ptr<Serie>& s1);
    Serie max2ref(const Serie& s0, const Serie& s1);
}
