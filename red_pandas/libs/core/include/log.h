#pragma once

#include "serie.h"

namespace rp {
    std::shared_ptr<Serie> log(const std::shared_ptr<Serie> &s0);

    Serie log(const Serie& s0);
}
