#pragma once

#include "serie.h"

namespace rp {
    std::shared_ptr<Serie> div2(const std::shared_ptr<Serie> &s0, const std::shared_ptr<Serie> &s1);

    Serie div2(const Serie& s0, const Serie& s1);

}
