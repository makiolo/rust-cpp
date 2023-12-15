#pragma once

#include "serie.h"

namespace rp
{
    std::shared_ptr<Serie> seq(int elements);

    Serie seqref(int elements);
}
