#pragma once

#include "serie.h"

namespace rp {
    std::shared_ptr<Serie> pdf(const std::shared_ptr<Serie> &s0);

    Serie pdf(const Serie& s0);
}
