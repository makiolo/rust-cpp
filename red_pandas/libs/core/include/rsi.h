#pragma once

#include "serie.h"

namespace rp
{
    // combinar con window2
    //
    // varios tipos de indicators
    // - close, period
    // - close, high, low, period
    // - high, low, period
    // General:
    // period, serie ...
    // o
    // dataframe, period <----- mejor opcion
    //
    // Interfaz de indicator
    // dataframe is OHCL
    dataframe rsi_indicator(const dataframe &data, int period);
    column_ptr rsi(const column_ptr &s0, int period = 14);
    Serie rsi(const Serie& s0, int period = 14);
}
