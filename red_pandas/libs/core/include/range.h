#pragma once

#include "serie.h"

namespace rp
{
    std::shared_ptr<Serie> range(int elements);

    Serie rangeref(int elements);
}
