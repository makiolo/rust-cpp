#include <catch_amalgamated.hpp>
#include <iostream>
#include "simulator.h"
#include "serie.h"
#include "exp.h"
#include "max0.h"
#include "sqrt.h"
#include "formulas/greeks.h"
#include "operators/less.h"

TEST_CASE("simulator_test1", "[example]")
{
    //
    // timestep major == column major
    // path major == row major
    // 12 combinations (fondo) and 100 timesteps (ancho) per path
    // total 50 paths (alto)
    //
    auto S = rp::array({48.40, 48.40, 48.40, 48.40, 48.40, 48.40, 48.40, 48.40, 48.40, 48.40, 48.40, 48.40});
    auto Vol = rp::array({0.18, 0.18, 0.18, 0.18, 0.18, 0.18, 0.18, 0.18, 0.18, 0.18, 0.18, 0.18});
    auto T = rp::array({56.0 / 365, 56.0 / 365, 56.0 / 365, 56.0 / 365, 56.0 / 365, 56.0 / 365, 112.0 / 365, 112.0 / 365, 112.0 / 365, 112.0 / 365, 112.0 / 365, 112.0 / 365});
    auto K = rp::array({44.0, 46.0, 48.0, 50.0, 52.0, 54.0, 44.0, 46.0, 48.0, 50.0, 52.0, 54.0});
    auto r = rp::array({0.09, 0.09, 0.09, 0.09, 0.09, 0.09, 0.09, 0.09, 0.09, 0.09, 0.09, 0.09});
    auto q = rp::array({0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06});

    // num paths = num estrategias
    // 12 prices (6 strikes x 2 tiempos) x Y paths x Z timesteps
    // size_t elements = S->size();
    size_t paths = 50;  // paths
    auto timesteps = rp::constant(100);  // timesteps

    auto mu = r - q;
    sim::Simulator sim(sim::Simulator::PRICE_VOLATILITY);
    for(size_t i = 0; i < paths; ++i)
    {
        sim.append_path(S, Vol, mu);
    }
    auto dt = T / timesteps;
    auto acum_dt = rp::zero();
    while(rp::all_less(acum_dt, T))
    {
        sim.update(dt);
        acum_dt = acum_dt + dt;
    }
    REQUIRE_THAT(acum_dt->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.1534246575, 0.1534246575, 0.1534246575, 0.1534246575, 0.1534246575,
              0.1534246575, 0.3068493151, 0.3068493151, 0.3068493151, 0.3068493151,
              0.3068493151, 0.3068493151 }
    )) );
    REQUIRE(acum_dt == T);

    rp::dataframe positive;
    rp::dataframe negative;
    size_t i = 0;
    for(const auto& path : sim.get_results())
    {
        const auto& last_price = path.at("price");
        // const auto& last_sigma = path.at("sigma");
        //
//        const auto& price0 = path.at("price_0");
//        const auto& price1 = path.at("price_1");
//        const auto& price2 = path.at("price_2");
//        const auto& price3 = path.at("price_3");
//        const auto& price4 = path.at("price_4");
//        const auto& price5 = path.at("price_5");
//        const auto& price6 = path.at("price_6");
//        const auto& price7 = path.at("price_7");
//        const auto& price8 = path.at("price_8");
//        const auto& price9 = path.at("price_9");
//        const auto& price10 = path.at("price_10");
//        const auto& price11 = path.at("price_11");
//        //
//        const auto& rsi0 = path.at("rsi_0");
//        const auto& rsi1 = path.at("rsi_1");
//        const auto& rsi2 = path.at("rsi_2");
//        const auto& rsi3 = path.at("rsi_3");
//        const auto& rsi4 = path.at("rsi_4");
//        const auto& rsi5 = path.at("rsi_5");
//        const auto& rsi6 = path.at("rsi_6");
//        const auto& rsi7 = path.at("rsi_7");
//        const auto& rsi8 = path.at("rsi_8");
//        const auto& rsi9 = path.at("rsi_9");
//        const auto& rsi10 = path.at("rsi_10");
//        const auto& rsi11 = path.at("rsi_11");
//        const auto& mean0 = rp::mean({price0, price1, price2, price3, price4, price5, price6, price7, price8, price9, price10, price11});
//        const auto& std0 = rp::std({price0, price1, price2, price3, price4, price5, price6, price7, price8, price9, price10, price11});
//        const auto& mean1 = rp::mean({rsi0, rsi1, rsi2, rsi3, rsi4, rsi5, rsi6, rsi7, rsi8, rsi9, rsi10, rsi11});
//        const auto& std1 = rp::std({rsi0, rsi1, rsi2, rsi3, rsi4, rsi5, rsi6, rsi7, rsi8, rsi9, rsi10, rsi11});
        const auto& last_minus_K = rp::max0(last_price - K);
        const auto& K_minus_last = rp::max0(K - last_price);   // last_price - K - last_minus_K
        positive.emplace_back(last_minus_K);
        negative.emplace_back(K_minus_last);
        ++i;
    }

    // Montecarlo
    auto to_present = rp::exp(-r * T);  // discount rate
    auto call_price = rp::mean_transpose(positive) * to_present;
    auto put_price = rp::mean_transpose(negative) * to_present;

    REQUIRE_THAT(call_price->to_vector(), Catch::Matchers::Approx(std::vector<double>(
         // { 4.6734025129, 2.9918930242, 1.6697017398, 0.7945994099, 0.3186441338, 0.1073720162, 5.0567811744, 3.5420187248, 2.3139887734, 1.4031334639, 0.7881237237, 0.41021632 }
            { 4.6843980754, 3.5374400689, 2.057458122, 1.1169314318, 0.5765243506,
              0.1932311898, 5.9109947355, 3.1607048729, 2.2829796567, 1.6357902127,
              0.6970431265, 1.054917427 }
    )) );
    REQUIRE_THAT(put_price->to_vector(), Catch::Matchers::Approx(std::vector<double>(
         // { 0.1135170557, 0.4045809211, 1.0549629906, 2.1524340148, 3.6490520926, 5.4103533291, 0.3412207868, 0.7719811557, 1.4894740227, 2.5241415317, 3.85465461, 5.4222700247 }
            { 0.1793876448, 0.4079707275, 1.0372449574, 2.6416417927, 3.5348094018,
              5.2006337123, 0.2306881733, 1.0792587313, 1.3937435568, 2.2486237341,
              2.8199966041, 5.0633032986 }
    )) );

    // Formulas
    auto duno = rp::d1(S, K, r, q, T, Vol);
    auto ddos = rp::d2(S, K, r, q, T, Vol);

    auto call_price_analythic = rp::CallPrice(S, K, r, q, T, Vol, duno, ddos);
    auto put_price_analythic = rp::PutPrice(S, K, r, q, T, Vol, duno, ddos);

    REQUIRE_THAT(call_price_analythic->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 4.6734025129, 2.9918930242, 1.6697017398, 0.7945994099, 0.3186441338, 0.1073720162, 5.0567811744, 3.5420187248, 2.3139887734, 1.4031334639, 0.7881237237, 0.41021632 }
    )) );
    REQUIRE_THAT(put_price_analythic->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.1135170557, 0.4045809211, 1.0549629906, 2.1524340148, 3.6490520926, 5.4103533291, 0.3412207868, 0.7719811557, 1.4894740227, 2.5241415317, 3.85465461, 5.4222700247 }
    )) );
}
