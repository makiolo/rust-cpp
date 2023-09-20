#pragma once

#include "serie.h"

namespace rp {
    std::shared_ptr<Serie> exp(const std::shared_ptr<Serie> &s0);

    Serie exp(const Serie& s0);
}
