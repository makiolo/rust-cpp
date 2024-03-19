//
// Created by n424613 on 21/10/2022.
//
#include <catch_amalgamated.hpp>
#include "serie.h"
#include "div.h"
#include "mul.h"
#include "sum.h"
#include "exp.h"
#include "log.h"
#include "pow.h"
#include "operators/less.h"
#include "operators/isfinite.h"
#include "cdf.h"
#include "pdf.h"
#include "ppf.h"
#include "sqrt.h"
#include "rsi.h"
#include "shift.h"
#include "diff.h"
#include "clean.h"
#include "range.h"
#include "rand_normal.h"
#include "formulas/greeks.h"
#include <functional>
#include "arrow.h"


TEST_CASE("sum ref", "[rp]")
{
    auto s1 = rp::array({1, 2, 3 , 4,  5,  6});
    auto s2 = rp::array({2, 4, 6 , 8, 10, 12});
    auto future_result = rp::sum_reactive(s1, s2);

    s1->write(0, 7);
    s2->write(0, 7);

    auto result = rp::calculate(future_result);
    
    REQUIRE_THAT(result->to_vector(), Catch::Matchers::Approx(std::vector<double>({14, 6, 9 , 12,  15, 18})) );

    s2->write(0, 3);
    auto result2 = rp::calculate(future_result);
    REQUIRE_THAT(result->to_vector(), Catch::Matchers::Approx(std::vector<double>({14, 6, 9 , 12,  15, 18})) );
}

TEST_CASE("sum", "[rp]")
{
    auto s1 = rp::array({1, 2, 3 , 4,  5,  6});
    auto s2 = rp::array({2, 4, 6 , 8, 10, 12});

    auto result = rp::calculate(s1 + s2);

    REQUIRE_THAT(result->to_vector(), Catch::Matchers::Approx(std::vector<double>({3, 6, 9 , 12,  15, 18})) );
}

TEST_CASE("sum - Vector N + scalar", "[rp]")
{
    auto s1 = std::make_shared<Serie>(rp::internal_array({1, 2, 3, 4, 5, 6}));
    auto s2 = std::make_shared<Serie>(4);

    auto result = rp::calculate(s1 + s2);

    REQUIRE_THAT(result->to_vector(), Catch::Matchers::Approx(std::vector<double>({5, 6, 7, 8, 9, 10})) );
}

TEST_CASE("sub", "[rp]")
{
    // make_serie = std::make_shared<column>(std::forward<array>(data));

    auto s1 = rp::array({1, 2, 3 , 4,  5,  6});
    auto s2 = rp::array({2, 4, 6 , 8, 10, 12});
    auto s3 = rp::array({2, 4, 6 , 8, 10, 12});

    // async code
    auto result = rp::calculate(((-s1 - s2) - s3) - s3 - s3);

    REQUIRE_THAT(result->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { -9.0, -18.0, -27.0, -36.0, -45.0, -54.0 }
    )) );
}

TEST_CASE("sub ref", "[rp]")
{
    Serie s1({1, 2, 3 , 4,  5,  6});
    Serie s2({2, 4, 6 , 8, 10, 12});
    Serie s3({2, 4, 6 , 8, 10, 12});

    auto result = rp::calculate(((-s1 - s2) - s3) - s3 - s3);

    REQUIRE_THAT(result->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { -9.0, -18.0, -27.0, -36.0, -45.0, -54.0 }
    )) );
}

TEST_CASE("sub - vector - scalar", "[rp]")
{
    auto s1 = std::make_shared<Serie>(rp::internal_array({1, 2, 3 , 4,  5,  6}));
    auto s2 = std::make_shared<Serie>(10);

    auto result = rp::calculate(s1 - s2);

    REQUIRE_THAT(result->to_vector(), Catch::Matchers::Approx(std::vector<double>({ -9.0, -8.0, -7.0, -6.0, -5.0, -4.0 })) );
}

TEST_CASE("pow", "[rp]")
{
    auto s1 = std::make_shared<Serie>(rp::internal_array({1, 2, 3 , 4, 5, 6}));
    auto s2 = std::make_shared<Serie>(rp::internal_array({1, 2, 3 , 4, 5, 6}));

    auto result = rp::calculate(rp::pow(s1, s2));

    REQUIRE_THAT(result->to_vector(), Catch::Matchers::Approx(std::vector<double>({ 1.0, 4.0, 27.0, 256.0, 3125.0, 46656.0 })) );
}

TEST_CASE("mul", "[rp]")
{
    auto s1 = std::make_shared<Serie>(rp::internal_array({1, 2, 3 , 4,  5,  6}));
    auto s2 = std::make_shared<Serie>(rp::internal_array({2, 4, 6 , 8, 10, 12}));

    auto result = rp::calculate(rp::mul2(s1, s2));

    REQUIRE_THAT(result->to_vector(), Catch::Matchers::Approx(std::vector<double>({2, 8, 18 , 32,  50, 72})) );
}

TEST_CASE("log", "[rp]")
{
    auto s1 = std::make_shared<Serie>(rp::internal_array({1, 2, 3 , 4,  5,  6}));

    auto v = rp::calculate(rp::log(s1));

    REQUIRE_THAT(v->to_vector(), Catch::Matchers::Approx(std::vector<double>({ 0.0, 0.6931471806, 1.0986122887, 1.3862943611, 1.6094379124, 1.7917594692 } )) );
}

TEST_CASE("exp", "[rp]")
{
    auto s1 = std::make_shared<Serie>(rp::internal_array({1, 2, 3 , 4,  5,  6}));

    auto v = rp::calculate(rp::exp(s1));

    REQUIRE_THAT(v->to_vector(), Catch::Matchers::Approx(std::vector<double>({ 2.7182818285, 7.3890560989, 20.0855369232, 54.5981500331, 148.4131591026, 403.4287934927 })) );
}

TEST_CASE("div", "[rp]")
{
    auto s1 = std::make_shared<Serie>(rp::internal_array({1, 2, 3 , 4,  5,  6}));
    auto s2 = std::make_shared<Serie>(rp::internal_array({2, 4, 6 , 8, 10, 12}));

    auto v = rp::calculate(rp::div2(s2, s1));

    REQUIRE_THAT(v->to_vector(), Catch::Matchers::Approx(std::vector<double>({ 2.0, 2.0, 2.0, 2.0, 2.0, 2.0 })) );
}

TEST_CASE("compare any < 1", "[rp]")
{
    auto s1 = rp::array({1.0, 2.0, 3.0 , 4.0,  5.0,  6.0});
    auto s2 = rp::array({1.5});

    auto v = rp::any_less(s1, s2);

    REQUIRE(v == true);
}

TEST_CASE("compare any < 2", "[rp]")
{
    auto s1 = rp::array({1.0, 2.0, 3.0 , 4.0,  5.0,  6.0});
    auto s2 = rp::array({0.5});

    auto v = rp::any_less(s1, s2);

    REQUIRE(v == false);
}

TEST_CASE("compare any < 3", "[rp]")
{
    auto s1 = rp::array({1.0, 2.0, 3.0 , 4.0,  5.0,  6.0});
    auto s2 = rp::array({6.5});

    auto v = rp::any_less(s1, s2);

    REQUIRE(v == true);
}

TEST_CASE("compare all < 1", "[rp]")
{
    auto s1 = rp::array({1.0, 2.0, 3.0 , 4.0,  5.0,  6.0});
    auto s2 = rp::array({1.5});

    auto v = rp::all_less(s1, s2);

    REQUIRE(v == false);
}

TEST_CASE("compare all < 2", "[rp]")
{
    auto s1 = rp::array({1.0, 2.0, 3.0 , 4.0,  5.0,  6.0});
    auto s2 = rp::array({0.5});

    auto v = rp::all_less(s1, s2);

    REQUIRE(v == false);
}

TEST_CASE("compare all < 3", "[rp]")
{
    auto s1 = rp::array({1.0, 2.0, 3.0 , 4.0,  5.0,  6.0});
    auto s2 = rp::array({6.5});

    auto v = rp::all_less(s1, s2);

    REQUIRE(v == true);
}

TEST_CASE("cdf", "[rp]")
{
    auto s1 = rp::array({0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 2.0, 3.0});

    auto v = rp::calculate(rp::cdf(s1));

    REQUIRE_THAT(v->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.5, 0.5398278373, 0.5792597094, 0.6179114222, 0.6554217416, 0.6914624613, 0.7257468822, 0.7580363478, 0.7881446014, 0.8159398747, 0.8413447461, 0.9772498681, 0.998650102 }
       )) );
}

TEST_CASE("cdf & ppf", "[rp]")
{
    auto s1 = rp::array({0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 2.0, 3.0});

    auto v = rp::calculate(rp::ppf(rp::cdf(s1)));

    REQUIRE_THAT(v->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 2.0, 3.0}
    )) );
}

TEST_CASE("call price", "[rp]")
{

//    BENCHMARK("rp::CallGreeksDelta") {
//                                         return rp::calculate(rp::CallGreeksDelta(S, K, r, T, Vol, duno, ddos));
//                                     };
    auto S = rp::array({50.0, 50.0, 50.0});
    auto K = rp::array({45.0, 40.1, 35.2});
    auto r = rp::array({0.0, 0.0, 0.0});
    auto q = rp::array({0.0, 0.0, 0.0});
    auto T = rp::array({0.8, 0.9, 1.0});
    auto Vol = rp::array({0.16, 0.16, 0.16});

    auto duno = rp::d1(S, K, r, q, T, Vol);
    auto ddos = rp::d2(S, K, r, q, T, Vol);

    auto result1 = rp::calculate(rp::CallPrice(S, K, r, q, T, Vol, duno, ddos));
    auto result2 = rp::calculate(rp::PutPrice(S, K, r, q, T, Vol, duno, ddos));

    auto call_delta = rp::calculate(rp::CallGreeksDelta(S, K, r, q, T, Vol, duno, ddos));
    auto call_gamma = rp::calculate(rp::CallGreeksGamma(S, K, r, q, T, Vol, duno, ddos));
    auto call_rho = rp::calculate(rp::CallGreeksRho(S, K, r, q, T, Vol, duno, ddos));
    auto call_vega = rp::calculate(rp::CallGreeksVega(S, K, r, q, T, Vol, duno, ddos));
    auto call_theta = rp::calculate(rp::CallGreeksTheta(S, K, r, q, T, Vol, duno, ddos));

    auto put_delta = rp::calculate(rp::PutGreeksDelta(S, K, r, q, T, Vol, duno, ddos));
    auto put_gamma = rp::calculate(rp::PutGreeksGamma(S, K, r, q, T, Vol, duno, ddos));
    auto put_rho = rp::calculate(rp::PutGreeksRho(S, K, r, q, T, Vol, duno, ddos));
    auto put_vega = rp::calculate(rp::PutGreeksVega(S, K, r, q, T, Vol, duno, ddos));
    auto put_theta = rp::calculate(rp::PutGreeksTheta(S, K, r, q, T, Vol, duno, ddos));

    auto call_market_price = rp::array({ 5.8, 10.2, 22.2 });
    auto put_market_price = rp::array({ 0.92, 0.21, 0.033 });
    auto volatility_call = rp::calculate(rp::CallImpliedvolatility(S, K, r, q, T, Vol, call_market_price));
    auto volatility_put = rp::calculate(rp::PutImpliedvolatility(S, K, r, q, T, Vol, put_market_price));

    REQUIRE_THAT(volatility_call->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.1512892709, 0.1724875085, 0.8273777488 }
    )) );
    REQUIRE_THAT(volatility_put->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.1607362679, 0.1581622422, 0.1597889955 }
    )) );

    REQUIRE_THAT(result1->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 5.9104309313, 10.1207538553, 14.8333190604 }
    )) );

    REQUIRE_THAT(result2->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.9104309313, 0.2207538553, 0.0333190604 }
    )) );

    REQUIRE_THAT(call_delta->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 79.0392289526, 93.693372413, 98.8505149812 }
    )) );
    REQUIRE_THAT(call_gamma->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 4.0233104946, 1.6318852229, 0.3761183283 }
    )) );
    REQUIRE_THAT(call_rho->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.2688734684, 0.3305333912, 0.3459193843 }
    )) );
    REQUIRE_THAT(call_vega->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.1287459358, 0.058747868, 0.0150447331 }
    )) );
    REQUIRE_THAT(call_theta->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { -0.0035272859, -0.0014306939, -0.0003297476 }
    )) );

    REQUIRE_THAT(put_delta->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { -20.9607710474, -6.306627587, -1.1494850188 }
    )) );
    REQUIRE_THAT(put_gamma->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 4.0233104946, 1.6318852229, 0.3761183283 }
    )) );
    REQUIRE_THAT(put_rho->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { -0.0911265316, -0.0303666088, -0.0060806157 }
    )) );
    REQUIRE_THAT(put_vega->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.1287459358, 0.058747868, 0.0150447331 }
    )) );
    REQUIRE_THAT(put_theta->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { -0.0035272859, -0.0014306939, -0.0003297476 }
    )) );
}

TEST_CASE("call price 2", "[rp]")
{
    auto S = rp::array({48.40, 48.40, 48.40, 48.40, 48.40, 48.40, 48.40, 48.40, 48.40, 48.40, 48.40, 48.40});
    auto K = rp::array({44.0, 46.0, 48.0, 50.0, 52.0, 54.0, 44.0, 46.0, 48.0, 50.0, 52.0, 54.0});
    auto r = rp::array({0.09, 0.09, 0.09, 0.09, 0.09, 0.09, 0.09, 0.09, 0.09, 0.09, 0.09, 0.09});
    auto q = rp::array({0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06, 0.06});
    auto T = rp::array({56.0/365, 56.0/365, 56.0/365, 56.0/365, 56.0/365, 56.0/365, 112.0/365, 112.0/365, 112.0/365, 112.0/365, 112.0/365, 112.0/365});
    auto Vol = rp::array({0.18, 0.18, 0.18, 0.18, 0.18, 0.18, 0.18, 0.18, 0.18, 0.18, 0.18, 0.18});
    auto call_market_price = rp::array({4.59, 2.99, 1.75, 0.93, 0.47, 0.23, 4.96, 3.52, 2.38, 1.55, 0.97, 0.6});
    auto put_market_price = rp::array({0.2, 0.58, 1.35, 2.53, 4.06, 5.84, 0.56, 1.13, 1.98, 3.14, 4.58, 6.21});

    auto ms2 = K->reshape(2, 6);

    REQUIRE(ms2.rank() == 2);

    for(size_t i=0; i != ms2.extent(0); i++) {

        for (size_t j = 0; j != ms2.extent(1); j++) {
            std::cout << "Strike i=" << i << ", j=" << j << " = " << ms2(i, j) << std::endl;
        }
        std::cout << "-----" << std::endl;
    }

    auto duno = rp::d1(S, K, r, q, T, Vol);
    auto ddos = rp::d2(S, K, r, q, T, Vol);

    auto result1 = rp::calculate(rp::CallPrice(S, K, r, q, T, Vol, duno, ddos));
    auto result2 = rp::calculate(rp::PutPrice(S, K, r, q, T, Vol, duno, ddos));

    auto call_delta = rp::calculate(rp::CallGreeksDelta(S, K, r, q, T, Vol, duno, ddos));
    auto call_gamma = rp::calculate(rp::CallGreeksGamma(S, K, r, q, T, Vol, duno, ddos));
    auto call_rho = rp::calculate(rp::CallGreeksRho(S, K, r, q, T, Vol, duno, ddos));
    auto call_vega = rp::calculate(rp::CallGreeksVega(S, K, r, q, T, Vol, duno, ddos));
    auto call_theta = rp::calculate(rp::CallGreeksTheta(S, K, r, q, T, Vol, duno, ddos));

    auto put_delta = rp::calculate(rp::PutGreeksDelta(S, K, r, q, T, Vol, duno, ddos));
    auto put_gamma = rp::calculate(rp::PutGreeksGamma(S, K, r, q, T, Vol, duno, ddos));
    auto put_rho = rp::calculate(rp::PutGreeksRho(S, K, r, q, T, Vol, duno, ddos));
    auto put_vega = rp::calculate(rp::PutGreeksVega(S, K, r, q, T, Vol, duno, ddos));
    auto put_theta = rp::calculate(rp::PutGreeksTheta(S, K, r, q, T, Vol, duno, ddos));

    auto volga = rp::calculate(rp::GreeksVolga(S, K, r, q, T, Vol, duno, ddos));

    auto volatility_call = rp::calculate(rp::CallImpliedvolatility(S, K, r, q, T, Vol, call_market_price));
    auto volatility_put = rp::calculate(rp::PutImpliedvolatility(S, K, r, q, T, Vol, put_market_price));

    REQUIRE_THAT(volatility_call->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            // { 0.1982695341, 0.2025283887, 0.2047436408, 0.2087901126, 0.2163141807, 0.2245910822, 0.201213111, 0.2020478448, 0.2042142323, 0.2080274597, 0.2114142454, 0.2164020152 }
            { 0.1372465478, 0.1796458561, 0.1909579064, 0.1991300314, 0.2088594775, 0.2184538717, 0.1621301691, 0.1774005573, 0.1864417146, 0.1941909975, 0.2000538642, 0.2067094504 }
    )) );
    REQUIRE_THAT(volatility_put->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            // { 0.2011836394, 0.2008591008, 0.2047436408, 0.2087901126, 0.2144819139, 0.2272837269, 0.201213111, 0.2031119141, 0.2042142323, 0.2070687857, 0.2125028973, 0.2177437958 }
            { 0.2087726987, 0.211333438, 0.2201982707, 0.2328260716, 0.2531174549, 0.2871161396, 0.2147974973, 0.2207878173, 0.2277728609, 0.2392680723, 0.2569441427, 0.2787615828 }
    )) );

    REQUIRE_THAT(result1->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 4.6734025129, 2.9918930242, 1.6697017398, 0.7945994099, 0.3186441338, 0.1073720162, 5.0567811744, 3.5420187248, 2.3139887734, 1.4031334639, 0.7881237237, 0.41021632 }
    )) );
    REQUIRE_THAT(result2->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.1135170557, 0.4045809211, 1.0549629906, 2.1524340148, 3.6490520926, 5.4103533291, 0.3412207868, 0.7719811557, 1.4894740227, 2.5241415317, 3.85465461, 5.4222700247 }
    )) );
    REQUIRE_THAT(call_delta->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 91.8306199014, 78.7147731299, 58.1005835687, 35.5850765918, 17.7916382267, 7.2535220383, 84.8150888341, 72.9326624102, 57.8421122028, 41.9216021528, 27.6708580147, 16.6478595146 }
    )) );
    REQUIRE_THAT(call_gamma->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 4.0346867247, 8.2635532195, 11.3110854204, 10.853915446, 7.607386032, 4.0348849755, 4.4413171357, 6.5607599928, 7.9122656704, 7.9796141413, 6.8699019987, 5.1396571945 }
    )) );
    REQUIRE_THAT(call_rho->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.0610210022, 0.053861348, 0.0405823266, 0.025205489, 0.0127227532, 0.0052215515, 0.1104464889, 0.0974473333, 0.0788037939, 0.0579543907, 0.0386770415, 0.023465834 }
    )) );
    REQUIRE_THAT(call_vega->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.0261016649, 0.0534595402, 0.0731749902, 0.070217413, 0.0492145871, 0.0261029475, 0.0574645714, 0.0848872642, 0.1023738999, 0.103245297, 0.0888871391, 0.066500137 }
    )) );
    REQUIRE_THAT(call_theta->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { -0.0066956621, -0.0109853231, -0.013659848, -0.0125046164, -0.0085386834, -0.0044571923, -0.0067448252, -0.0088492448, -0.0099569083, -0.0096181948, -0.0080491541, -0.0059048773 }
            // { -2.4439166837, -4.0096429489, -4.9858445114, -4.5641849683, -3.1166194472, -1.6268752025, -2.461861187, -3.2299743494, -3.6342715247, -3.5106410858, -2.9379412587, -2.1552802193 }
    )) );

    REQUIRE_THAT(put_delta->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { -7.2530562245, -20.3689029961, -40.9830925572, -63.4985995341, -81.2920378993, -91.8301540877, -13.3606599122, -25.243086336, -40.3336365435, -56.2541465934, -70.5048907315, -81.5278892316 }
    )) );
    REQUIRE_THAT(put_gamma->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 4.0346867247, 8.2635532195, 11.3110854204, 10.853915446, 7.607386032, 4.0348849755, 4.4413171357, 6.5607599928, 7.9122656704, 7.9796141413, 6.8699019987, 5.1396571945 }
    )) );
    REQUIRE_THAT(put_rho->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { -0.0055601039, -0.015746172, -0.0320516073, -0.0504548588, -0.0659640086, -0.0764916242, -0.0208896269, -0.0398586059, -0.0644719688, -0.0912911953, -0.116538368, -0.137719399 }
    )) );
    REQUIRE_THAT(put_vega->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.0261016649, 0.0534595402, 0.0731749902, 0.070217413, 0.0492145871, 0.0261029475, 0.0574645714, 0.0848872642, 0.1023738999, 0.103245297, 0.0888871391, 0.066500137 }
    )) );
    REQUIRE_THAT(put_theta->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { -0.0038783874, -0.0076816604, -0.0098697973, -0.0082281777, -0.0037758569, 0.0007920222, -0.0040020541, -0.0056267558, -0.0062547013, -0.0054362698, -0.0033875113, -0.0007635165 }
    )) );

    REQUIRE_THAT(volga->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.2910247939, 0.1834075852, 0.013107147, 0.060690642, 0.2476072496, 0.3207233863, 0.3499747271, 0.1699581422, 0.0161145128, 0.0299432802, 0.1930356485, 0.3727599145 }
    )) );
}

TEST_CASE("agg_test", "[agg]")
{
    auto price = rp::array({8, 8.1, 9, 8, 9, 9, 7, 8, 7, 7, 8, 7, 8, 6, 7, 8, 9, 5, 7, 10, 7, 6, 8, 5, 6, 8});
    auto vol = rp::array({28, 28.1, 29, 28, 29, 29, 27, 28, 27, 27, 28, 27, 28, 26, 27, 28, 29, 25, 27, 210, 27, 26, 28, 25, 26, 28});
    auto third = rp::array({128, 128.1, 129, 218, 129, 129, 127, 128, 27, 27, 28, 27, 28, 26, 27, 28, 29, 25, 27, 210, 27, 26, 28, 25, 26, 28});

    const double* data = price->raw_data();
    const double* data2 = vol->raw_data();
    const double* data3 = third->raw_data();

    // desfavorable a cache
    std::vector<double> sum_horizontal;

    // favorable a cache
    double sum_data = 0.0;
    double sum_data2 = 0.0;
    double sum_data3 = 0.0;

    for(size_t i=0; i < price->size(); ++i)
    {
        // sumatorio desfavorable a cache
        sum_horizontal.emplace_back(data[i] + data2[i] + data3[i]);

        // sumatorio favorable a cache
        sum_data += data[i];
        sum_data2 += data2[i];
        sum_data3 += data3[i];
    }

    // desfavorable a cache
    auto result_horizontal = rp::array(sum_horizontal);
    auto result1 = rp::agg_transpose({price, vol, third}, std::bind(rp::sum2, std::placeholders::_1, std::placeholders::_2));
    REQUIRE(result_horizontal == result1);

    // favorable a cache
    auto result_vertical = rp::array({sum_data, sum_data2, sum_data3});
    auto result2 = rp::sum({price, vol, third});
    REQUIRE(result_vertical == result2);
}

TEST_CASE("nan_default", "[nan0]")
{
    auto price = rp::array({1,1,1,1,1});
    auto vol = rp::array({2,2,2,2});
    auto third = rp::array({3,3,3});

    REQUIRE(rp::clean(rp::sum({price, vol, third}), true, -999.0) == rp::array({5, -999.0, -999.0}));

    // norm_nans to nans
    auto filled = rp::norm_nans({price, vol, third});

    REQUIRE(rp::clean(rp::agg(filled, std::bind(rp::sum2, std::placeholders::_1, std::placeholders::_2)), true, -999.0) == rp::array({5, -999.0, -999.0}));
    REQUIRE(rp::clean(rp::agg_transpose(filled, std::bind(rp::sum2, std::placeholders::_1, std::placeholders::_2)), true, -999.0) == rp::array({6, 6, 6, -999.0, -999.0}));

    // REQUIRE(rp::clean(rp::agg(filled, true), true, -999.0) == rp::array({5, 8, 9}));
    // REQUIRE(rp::clean(rp::agg_transpose(filled, true), true, -999.0) == rp::array({6, 6, 6, 3, 1}));

    REQUIRE(rp::clean(rp::agg(filled, std::bind(rp::sum2, std::placeholders::_1, std::placeholders::_2)), false) == rp::array({5, }));
    REQUIRE(rp::clean(rp::agg_transpose(filled, std::bind(rp::sum2, std::placeholders::_1, std::placeholders::_2)), false) == rp::array({6, 6, 6, }));
}

TEST_CASE("refill_nan_with_zeros", "[nan1]")
{
    auto price = rp::array({1,1,1,1,1});
    auto vol = rp::array({2,2,2,2});
    auto third = rp::array({3,3,3});

    // norm_nans to zeros
    auto filled = rp::norm_fill({price, vol, third});

    REQUIRE(rp::agg(filled, std::bind(rp::sum2, std::placeholders::_1, std::placeholders::_2)) == rp::array({5, 8, 9}));
    REQUIRE(rp::agg_transpose(filled, std::bind(rp::sum2, std::placeholders::_1, std::placeholders::_2)) == rp::array({6, 6, 6, 3, 1}));
}

TEST_CASE("drop_rows_with_nan", "[nan2]")
{
    auto price = rp::array({1,1,1,1,1});
    auto vol = rp::array({2,2,2,2});
    auto third = rp::array({3,3,3});

    // norm_nans to drop nans
    auto dropped = rp::norm_drop({price, vol, third});

    REQUIRE(rp::sum(dropped) == rp::array({3, 6, 9}));
    REQUIRE(rp::sum_transpose(dropped) == rp::array({6, 6, 6}));
    REQUIRE(rp::mean(dropped) == rp::array({1, 2, 3}));
    REQUIRE(rp::mean_transpose(dropped) == rp::array({2, 2, 2}));
    //
    REQUIRE(rp::var(dropped) == rp::array({0, 0, 0}));
    REQUIRE(rp::var_transpose(dropped) == rp::array({1, 1, 1}));
    REQUIRE(rp::std(dropped) == rp::array({0, 0, 0}));
    REQUIRE(rp::std_transpose(dropped) == rp::array({1, 1, 1}));
    //
    REQUIRE(rp::mul(dropped) == rp::array({1, 8, 27}));
    REQUIRE(rp::mul_transpose(dropped) == rp::array({6, 6, 6}));
}


TEST_CASE("test_var_std", "[stats]")
{
    auto price = rp::array({5,1,5});
    auto vol = rp::array({3,6,2});
    auto third = rp::array({3,6,4});

    REQUIRE(rp::var({price, vol, third}) == rp::array({5.33333, 4.33333, 2.33333}));
    REQUIRE(rp::var_transpose({price, vol, third}) == rp::array({1.33333, 8.33333, 2.33333}));
    REQUIRE(rp::std({price, vol, third}) == rp::array({2.3094, 2.08167, 1.52753}));
    REQUIRE(rp::std_transpose({price, vol, third}) == rp::array({1.1547, 2.88675, 1.52753}));
}

TEST_CASE("test_volatility", "[stats]")
{
    auto price = rp::array({40,42,39,43,37,34,39,45,41});

    REQUIRE(rp::std({rp::array({1,1,1,1,1,1,1,2,2,2,2,1,1,1,1,2,2,1,1,2,1,2,1,2,3}), }) * rp::sqrt(rp::constant(252)) == rp::array({9.25635}));

    REQUIRE(rp::mean({price, }) == rp::array({40}));
    REQUIRE(rp::mean_transpose({price, }) == rp::array({40,42,39,43,37,34,39,45,41}));
    REQUIRE(rp::std({price, }) == rp::array({3.27872}));
    REQUIRE(rp::std({price, }, false) == rp::array({3.09121}));

    auto price2 = rp::array({81, 83, 79, 85, 77, 70, 54, 60, 65, 86, 95});

    REQUIRE(rp::mean({price2}) == rp::array({75.9091}));
    // REQUIRE(rp::std({price2}) == rp::array({12.3406}));
    // REQUIRE(rp::std({price2}, false) == rp::array({11.7663}));

    // dropeando datos, baja la media
    auto dropped = rp::norm_drop({price, price2});

    /*
    REQUIRE(rp::mean(dropped) == rp::array({40, 72.6667}));
    REQUIRE(rp::std(dropped) == rp::array({3.27872, 10.9659}));
    REQUIRE(rp::std(dropped, false) == rp::array({3.09121, 10.3387}));
    */
    // TODO:
    /*
    REQUIRE(rp::mean_transpose(dropped) == rp::array({40, 72.6667}));
    REQUIRE(rp::std_transpose(dropped) == rp::array({3.27872, 10.9659}));
    REQUIRE(rp::std_transpose(dropped, false) == rp::array({3.09121, 10.3387}));
    */

    // rellenando la media, baja la desviacion tipica
    // rellenar con una distribucion normal (con media y sigma que lleva a medio camino hasta llegar a ser como el más largo)
    // norm_stochastic()

    /*
    auto filled = rp::norm_fill({price, price2}, 40.0);

    REQUIRE(rp::mean(filled) == rp::array({40, 75.9091}));
    REQUIRE(rp::std(filled) == rp::array({2.93258, 12.3406}));
    REQUIRE(rp::std(filled, false) == rp::array({2.7961, 11.7663}));
    */
}

TEST_CASE("drop_rows_with_nan2", "[nan3]")
{
    auto price = rp::array({NAN,1,1,1,1});
    auto vol = rp::array({2,2,2,2});
    auto third = rp::array({3,3,3});

    // norm_nans to drop nans
    auto dropped = rp::norm_drop({price, vol, third});

    REQUIRE(rp::agg(dropped, std::bind(rp::sum2, std::placeholders::_1, std::placeholders::_2)) == rp::array({2, 4, 6}));
    REQUIRE(rp::agg_transpose(dropped, std::bind(rp::sum2, std::placeholders::_1, std::placeholders::_2)) == rp::array({6, 6}));
    REQUIRE(rp::agg(dropped, std::bind(rp::mul2, std::placeholders::_1, std::placeholders::_2)) == rp::array({1, 4, 9}));
    REQUIRE(rp::agg_transpose(dropped, std::bind(rp::mul2, std::placeholders::_1, std::placeholders::_2)) == rp::array({6, 6}));
}

TEST_CASE("talib_1", "[talib]")
{
    auto price = rp::array({8, 8.1, 9, 8, 9, 9, 7, 8, 7, 7, 8, 7, 8, 6, 7, 8, 9, 5, 7, 10, 7, 6, 8, 5, 6, 8});

    int period = 21; // RSI parameter
    auto rsi = rp::rsi(price, period);
    std::cout << "price: " << price << std::endl;
    std::cout << "price sliced 6: " << price->sub(6) << std::endl;
    std::cout << "price sliced 2, 6: " << price->sub(2, 6) << std::endl;
    std::cout << "price sliced 2, 6, 2: " << price->sub(2, 6, 2) << std::endl;

    const auto& internal_vector = price->data();
    auto len = price->size();

    auto result = rp::calculate(rsi);
    const double* raw_data = result->raw_data();

    REQUIRE(len > period);
    REQUIRE(result->size() == len);

    for(int i=0; i<period; ++i)
    {
        REQUIRE(std::isnan(raw_data[i]));
    }
    for(int i=period; i<len; ++i)
    {
        REQUIRE(std::isfinite(raw_data[i]));
    }
    REQUIRE_THAT(result->sub(period, len)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
        { 46.4285714286, 50.1661129568, 45.1994312654, 47.0347640866, 50.5151044484 }
    )) );
}

TEST_CASE("isfinite", "[isfinite]")
{
    REQUIRE(rp::any_isfinite(rp::array({1,2,3,4,5,6})) == true);
    REQUIRE(rp::all_isfinite(rp::array({1,2,3,4,5,6})) == true);
    REQUIRE(rp::any_isfinite(rp::array({1,2,3,4,5,6, NAN})) == true);
    REQUIRE(rp::all_isfinite(rp::array({1,2,3,4,5,6, NAN})) == false);
}

TEST_CASE("shift_1", "[shift]")
{
    int period = 3;
    auto price = rp::array({1, 2, 3, 4, 5, 5, 4, 3, 2, 1});
    auto shifted = rp::shift(price, period);
    auto diff = price - shifted;
    auto returns = (price / shifted) - rp::constant(1.0);
    auto returns_log = rp::log(price / shifted);

    REQUIRE_THAT(rp::clean(diff)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            // period 1
            // {1, 1, 1, 1, 0, -1, -1, -1, -1}
            // period 3
            { 3.0, 3.0, 2.0, 0.0, -2.0, -3.0, -3.0 }
    )) );
    REQUIRE_THAT(rp::agg({rp::calculate(rp::clean(shifted))}, std::bind(rp::sum2, std::placeholders::_1, std::placeholders::_2))->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 24.0 }
    )) );
    REQUIRE_THAT(rp::clean(shifted)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            // period 1
            // { 1, 2, 3, 4, 5, 5, 4, 3, 2 }
            // period 3
            { 1.0, 2.0, 3.0, 4.0, 5.0, 5.0, 4.0 }
    )) );
    REQUIRE_THAT(rp::clean(returns)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            // period 1
            // { 1.0, 0.5, 0.3333333333, 0.25, 0.0, -0.2, -0.25, -0.3333333333, -0.5 }
            // period 3
            { 3.0, 1.5, 0.6666666667, 0.0, -0.4, -0.6, -0.75 }
    )) );
    REQUIRE_THAT(rp::clean(returns_log)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            // period 1
            //{   0.6931471806,  0.4054651081,  0.2876820725,  0.2231435513, 0.0,
            //   -0.2231435513, -0.2876820725, -0.4054651081, -0.6931471806 }
            // period 3
            { 1.3862943611, 0.9162907319, 0.5108256238, 0.0, -0.5108256238,
              -0.9162907319, -1.3862943611 }

    )) );
    size_t count = price->size();
    REQUIRE(rp::calculate(shifted * rp::exp(returns_log))->sub(period, count) == price->sub(period, count));
    REQUIRE(rp::calculate(shifted * (rp::constant(1.0) + returns))->sub(period, count) == price->sub(period, count));
}

TEST_CASE("diff_1", "[diff]")
{
    int period = 1;
    auto price = rp::array({1, 2, 3, 4, 5, 5, 4, 3, 2, 1});
    auto shifted = rp::shift(price, period);
    auto diff2 = price - shifted;
    REQUIRE_THAT(rp::clean(diff2)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 1.0, 1.0, 1.0, 1.0, 0.0, -1.0, -1.0, -1.0, -1.0 }
    )) );
    auto diff1 = rp::diff(price, period);
    REQUIRE_THAT(rp::clean(diff1)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 1.0, 1.0, 1.0, 1.0, 0.0, -1.0, -1.0, -1.0, -1.0 }
    )) );
    REQUIRE(rp::clean(diff1) == rp::clean(diff2));
}

TEST_CASE("diff_2", "[diff]")
{
    int period = 3;
    auto price = rp::array({1, 2, 3, 4, 5, 5, 4, 3, 2, 1});
    auto diff1 = rp::diff(price, period);
    REQUIRE_THAT(rp::clean(diff1)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 3.0, 3.0, 2.0, 0.0, -2.0, -3.0, -3.0 }
    )) );
}

TEST_CASE("shift_3 negative", "[diff]")
{
    auto price = rp::array({10, 10, 30, 50, 0});

    REQUIRE_THAT(rp::clean(rp::shift(price, +2), true, -999.0)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { -999.0, -999.0, 10.0, 10.0, 30.0 }
    )) );
    REQUIRE_THAT(rp::clean(rp::shift(price, +1), true, -999.0)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { -999.0, 10.0, 10.0, 30.0, 50.0 }
    )) );
    REQUIRE_THAT(rp::clean(rp::shift(price, 0), true, -999.0)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            {10, 10, 30, 50, 0}
    )) );
    REQUIRE_THAT(rp::clean(rp::shift(price, -1), true, -999.0)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 10.0, 30.0, 50.0, 0.0, -999.0 }
    )) );
    REQUIRE_THAT(rp::clean(rp::shift(price, -2), true, -999.0)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 30.0, 50.0, 0.0, -999.0, -999.0 }
    )) );
}

TEST_CASE("diff_3 negative", "[diff]")
{
    auto price = rp::array({10, 10, 30, 50, 0});
    auto process1 = rp::diff(price, +1);
    REQUIRE_THAT(rp::clean(process1, true, -999.0)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { -999.0, 0.0, 20.0, 20.0, -50.0 }
    )) );
    auto process2 = rp::diff(process1, -1);
    REQUIRE_THAT(rp::clean(process2, true, -999.0)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { -999.0, -20.0, 0.0, 70.0, -999.0 }
    )) );

    REQUIRE_THAT(rp::clean(rp::diff(price, +2), true, -999.0)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { -999.0, -999.0, 20.0, 40.0, -30.0 }
    )) );
    REQUIRE_THAT(rp::clean(rp::diff(price, +1), true, -999.0)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { -999.0, 0.0, 20.0, 20.0, -50.0 }
    )) );
    REQUIRE_THAT(rp::clean(rp::diff(price, 0), true, -999.0)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.0, 0.0, 0.0, 0.0, 0.0 }
    )) );
    REQUIRE_THAT(rp::clean(rp::diff(price, -1), true, -999.0)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.0, -20.0, -20.0, 50.0, -999.0 }
    )) );
    REQUIRE_THAT(rp::clean(rp::diff(price, -2), true, -999.0)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { -20.0, -40.0, 30.0, -999.0, -999.0 }
    )) );
}

TEST_CASE("test range", "[range]")
{
    auto generated = rp::seq(20) * rp::constant(3.0);
    REQUIRE_THAT(generated->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.0, 3.0, 6.0, 9.0, 12.0, 15.0, 18.0, 21.0, 24.0, 27.0, 30.0, 33.0, 36.0,
              39.0, 42.0, 45.0, 48.0, 51.0, 54.0, 57.0 }
    )) );

    auto generated2 = rp::seq(-20);
    REQUIRE_THAT(generated2->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.0, -1.0, -2.0, -3.0, -4.0, -5.0, -6.0, -7.0, -8.0, -9.0, -10.0, -11.0,
              -12.0, -13.0, -14.0, -15.0, -16.0, -17.0, -18.0, -19.0 }
    )) );

//    auto range2 = rp::calculate(rp::seq(100));
//    for(auto n : range2->data())
//        std::cout << "n = " << n << std::endl;
}


TEST_CASE("test max", "[max]")
{
    auto price = rp::array({1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1});
    auto price2 = rp::array({1,2,33,4,5,6,7,8,-9,10,9,8,7,66,5,4,3,2,1});
    auto max_price = rp::max({price, price2});
    REQUIRE_THAT(max_price->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 10.0, 66.0 }
    )) );
    auto min_price = rp::min({price, price2});
    REQUIRE_THAT(min_price->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 1.0, -9.0 }
    )) );

    auto max_price_t = rp::max_transpose({price, price2});
    REQUIRE_THAT(max_price_t->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 1.0, 2.0, 33.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 9.0, 8.0, 7.0,
              66.0, 5.0, 4.0, 3.0, 2.0, 1.0 }
    )) );
    auto min_price_t = rp::min_transpose({price, price2});
    REQUIRE_THAT(min_price_t->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, -9.0, 10.0, 9.0, 8.0, 7.0,
              6.0, 5.0, 4.0, 3.0, 2.0, 1.0 }
    )) );
}

rp::dataframe multi_shift(const rp::column_ptr& data, const int Nfrom, const int Nto, const int Nstep, bool negative = false)
{
    rp::dataframe dataset;
    dataset.emplace_back(data);
    if(!negative)
    {
        for (int i = Nfrom; i < Nto; i += Nstep)
            dataset.emplace_back(rp::shift(data, double(i)));
    }
    else  // negative
    {
        for (int i = Nfrom; i < Nto; i += Nstep)
            dataset.emplace_back(rp::shift(data, double(-i)));
    }
    dataset = rp::norm_drop(dataset);
    return dataset;
}

rp::dataframe calculate_returns(
        rp::dataframe& data_shifts,
        bool negative = false)
{
    rp::dataframe data_returns;
    if(!negative)
    {
        for (size_t i = 1; i < data_shifts.size(); ++i)
            data_returns.emplace_back((data_shifts[0] / data_shifts[i]) - rp::one());
    }
    else
    {
        for(size_t i = 1; i < data_shifts.size(); ++i)
            data_returns.emplace_back((data_shifts[i] / data_shifts[0]) - rp::one());
    }
    return data_returns;
}

/*
TEST_CASE("reward ratio", "[reward]")
{
    int elements = 500;
    double initial_price = 40.0;

    // model
    auto close = rp::constant(initial_price) + (rp::constant(0.5) * rp::rand_normal(elements));
    auto high = close + (rp::constant(0.5) * rp::rand_normal(elements));
    auto low = close - (rp::constant(0.5) * rp::rand_normal(elements));

    const size_t Nfrom = 1;
    const size_t Nto = 200;
    const size_t Nstep = 7;

    // vectores de shifts en la direccion del vector
    rp::dataframe close_shift_past = multi_shift(close, Nfrom, Nto, Nstep);
    rp::dataframe close_shift_future = multi_shift(close, Nfrom, Nto, Nstep, true);

    rp::dataframe low_shift_past = multi_shift(low, Nfrom, Nto, Nstep);
    rp::dataframe low_shift_future = multi_shift(low, Nfrom, Nto, Nstep, true);

    rp::dataframe high_shift_past = multi_shift(high, Nfrom, Nto, Nstep);
    rp::dataframe high_shift_future = multi_shift(high, Nfrom, Nto, Nstep, true);

    // returns de cada shift
    rp::dataframe close_past_returns = calculate_returns(close_shift_past);
    rp::dataframe close_future_returns = calculate_returns(close_shift_future, true);

    rp::dataframe high_past_returns = calculate_returns(high_shift_past);
    rp::dataframe high_future_returns = calculate_returns(high_shift_future, true);

    rp::dataframe low_past_returns = calculate_returns(low_shift_past);
    rp::dataframe low_future_returns = calculate_returns(high_shift_future, true);

    // max / min del mismo slice de tiempo pero de múltiples shifts
    rp::column_ptr close_past_max = rp::max_transpose(close_past_returns);
    rp::column_ptr close_future_max = rp::max_transpose(close_future_returns);
    rp::column_ptr close_past_min = rp::min_transpose(close_past_returns);
    rp::column_ptr close_future_min = rp::min_transpose(close_future_returns);

    rp::column_ptr high_past_max = rp::max_transpose(high_past_returns);
    rp::column_ptr high_future_max = rp::max_transpose(high_future_returns);
    rp::column_ptr high_past_min = rp::min_transpose(high_past_returns);
    rp::column_ptr high_future_min = rp::min_transpose(high_future_returns);

    rp::column_ptr low_past_max = rp::max_transpose(low_past_returns);
    rp::column_ptr low_future_max = rp::max_transpose(low_future_returns);
    rp::column_ptr low_past_min = rp::min_transpose(low_past_returns);
    rp::column_ptr low_future_min = rp::min_transpose(low_future_returns);

    // MAX in window
    auto max_window = rp::max(rp::window(rp::calculate(close_past_max), 32));
    // MAX with rolling window 32
    REQUIRE_THAT(rp::calculate(max_window)->sub(0, 40)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0529381034, 0.0469609286, 0.0469609286, 0.0469609286, 0.0469609286, 0.0469609286, 0.0469609286, 0.0469609286, 0.0469609286, 0.0451235588, 0.0451235588, 0.0451235588, 0.0451235588, 0.0444269453, 0.0444269453 }
    )) );

    // MAX
    REQUIRE_THAT(rp::calculate(close_past_max)->sub(0, 20)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.0258689232, 0.0484766717, 0.0187171205, 0.0282448508, 0.0245774736, 0.0307526398, 0.0100389621, 0.0356671363, 0.0428072557, 0.0108868787, 0.0138742482, 0.0215452112, 0.0235368787, 0.0288980478, 0.0123663875, 0.0259202918, 0.0413459392, 0.0340503256, 0.0362241492, 0.0173117154 }
    )) );
    // MIN
    REQUIRE_THAT(rp::calculate(close_past_min)->sub(0, 20)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { -0.0204889775, -0.0174464303, -0.0376821373, -0.0272928378, -0.0312477017,
              -0.0091217642, -0.0436542748, -0.011133535, -0.022759381, -0.0450788733, -0.0408872536, -0.0341147481, -0.0160583854, -0.02579773, -0.0251217607, -0.0385845749, -0.0163061183, -0.021800929, -0.0202356074, -0.0220427299 }
    )) );
    // spread
    REQUIRE_THAT(rp::calculate(close_past_max - close_past_min)->sub(0, 20)->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.0463579007, 0.065923102, 0.0563992578, 0.0555376886, 0.0558251753, 0.039874404, 0.0536932369, 0.0468006713, 0.0655666367, 0.0559657519, 0.0547615018, 0.0556599593, 0.0395952641, 0.0546957777, 0.0374881482, 0.0645048668, 0.0576520576, 0.0558512546, 0.0564597566, 0.0393544453 }
    )) );

    auto close_avg = rp::mean({close_future_max, close_future_min, close_past_max, close_past_min});
    REQUIRE_THAT(close_avg->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            // high: future_max, future_min, past_max, past_min
            { 0.027332981, -0.025189269, 0.0266006233, -0.0258934626 }
    )) );

    auto high_avg = rp::mean({high_future_max, high_future_min, high_past_max, high_past_min});
    REQUIRE_THAT(high_avg->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            // high: future_max, future_min, past_max, past_min
            { 0.0360577274, -0.0363530711, 0.0386394198, -0.0338028905 }
    )) );

    auto low_avg = rp::mean({low_future_max, low_future_min, low_past_max, low_past_min});
    REQUIRE_THAT(low_avg->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            // low: future_max, future_min, past_max, past_min
            { 0.0360577274, -0.0363530711, 0.0353150984, -0.0358213258 }
    )) );
    auto low_std = rp::std({low_future_max, low_future_min, low_past_max, low_past_min});
    REQUIRE_THAT(low_std->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            // low: future_max, future_min, past_max, past_min
            { 0.0218297438, 0.0179192626, 0.020207715, 0.0177904648 }
    )) );

    auto close_returns_avg = rp::onehundred() * rp::mean({
        rp::mean(close_past_returns),
        rp::mean(close_future_returns),
        rp::mean(high_past_returns),
        rp::mean(high_future_returns),
        rp::mean(low_past_returns),
        rp::mean(low_future_returns),
    });
    auto close_returns_std = rp::onehundred() * rp::std({
          rp::std(close_past_returns),
          rp::std(close_future_returns),
          rp::std(high_past_returns),
          rp::std(high_future_returns),
          rp::std(low_past_returns),
          rp::std(low_future_returns),
    });
    REQUIRE_THAT(close_returns_avg->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.0067139172, 0.0792578769,
              0.024373667,  0.094136811,
              0.0454708367, 0.094136811 }
    )) );
    REQUIRE_THAT(close_returns_std->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 0.0542121113, 0.0459012721,
              0.0625369175, 0.0744082421,
              0.0672672074, 0.0744082421 }
    )) );
}
*/

TEST_CASE("moving average 20", "[ma]")
{
    int elements = 500;
    double initial_price = 40.0;
    auto close = rp::constant(initial_price) + (rp::constant(0.5) * rp::rand_normal(elements));

    auto dataset = rp::mapcalculate( rp::window( rp::calculate(close), 20) );
    auto avg = rp::mean({rp::mean(dataset)});
    REQUIRE_THAT(avg->to_vector(), Catch::Matchers::Approx(std::vector<double>(
            { 40.0026154698 }
    )) );
}
// make rp::linspace(-5, +5, 0.1)
// make rp::where(s > 5, 1, 0)

// usar callbacks de python, combinando swig con ctypes
// leer: https://stackoverflow.com/questions/34445045/passing-python-functions-to-swig-wrapped-c-code


TEST_CASE("arrow test", "[rp]")
{
    rp::Arrow test;
    test.append(1);
    test.append(2);
    test.append(3);
    test.finish();
    // std::cout << "test: " << test.to_string() << std::endl;
}
