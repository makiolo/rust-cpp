#pragma once

#include "serie.h"

namespace rp
{
    std::shared_ptr<Serie> min0(const std::shared_ptr<Serie> &s0);
    Serie min0ref(const Serie& s0);

    std::shared_ptr<Serie> min2(const std::shared_ptr<Serie>& s0, const std::shared_ptr<Serie>& s1);
    Serie min2ref(const Serie& s0, const Serie& s1);
}
