#pragma once

#include "serie.h"

namespace rp {
    std::shared_ptr<Serie> erf(const std::shared_ptr<Serie> &s0);

    Serie erf(const Serie& s0);
}
