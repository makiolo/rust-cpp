#pragma once

#include "serie.h"

namespace rp {
    std::shared_ptr<Serie> ppf(const std::shared_ptr<Serie> &s0);

    Serie ppf(const Serie& s0);
}
