//
// Created by n424613 on 04/11/2023.
//

#include <catch_amalgamated.hpp>
#include <memory>
#include "serie.h"
#include "sum.h"
#include "rand_normal.h"
#include "formulas/greeks.h"
#include "formulas/volatility.h"


TEST_CASE("parkinson bench 1", "[rp]")
{
    // period volatility
    int period = 30;
    double initial_price = 100;
    int elements = 150;
    auto close = rp::constant(initial_price) +
                 (rp::constant(12.0) * rp::rand_normal(elements));
    auto high = rp::calculate(close + (rp::constant(3.0) * rp::rand_normal(elements)));
    auto low = rp::calculate(close - (rp::constant(3.0) * rp::rand_normal(elements)));

//    auto high_win = rp::window(high, period);
//    auto low_win = rp::window(low, period);

//    size_t len = high_win.size();

//    BENCHMARK("parkinson bench") {
//
//         auto *newdata = new double[len];
//         for (size_t i = 0; i < len; ++i) {
//             newdata[i] = rp::parkinson(period, high_win[i], low_win[i])->data()[0];
//         }
//         auto parkinson = std::make_shared<Serie>(newdata, len, true);
//     };
}

TEST_CASE("parkinson bench 2", "[rp]")
{
    // period volatility
    int period = 30;
    double initial_price = 100;
    int elements = 150;
    auto close = rp::constant(initial_price) +
                 (rp::constant(12.0) * rp::rand_normal(elements));
    auto high = rp::calculate(close + (rp::constant(3.0) * rp::rand_normal(elements)));
    auto low = rp::calculate(close - (rp::constant(3.0) * rp::rand_normal(elements)));

//    auto high_win = rp::window(high, period);
//    auto low_win = rp::window(low, period);

//    size_t len = high_win.size();

//    BENCHMARK("parkinson bench") {
//
//         rp::dataframe results;
//         for (size_t i = 0; i < len; ++i) {
//             results.emplace_back(rp::parkinson(period, high_win[i], low_win[i]));
//         }
//         auto results2 = rp::transpose(results);
//     };
}
