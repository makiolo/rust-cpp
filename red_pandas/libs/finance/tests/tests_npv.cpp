//
// Created by n424613 on 21/10/2022.
//
#include <iostream>
#include <catch_amalgamated.hpp>
#include "npv.h"

using namespace qs;

//// TESTS ///////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("bond_npv", "[fv]") {

    using namespace date;

// Comparado con: https://mathcracker.com/es/calculadora-valor-bonos#results
// valor presente de un bono
// valorar un bono que da un yield "seguro" haciendo otros proyectos risk free
    double npv = bond_npv(
            // face value
            16000,
            // cupon
            100,
            InterestRate(0.06),
            // calendar
            Schedule( 20));

    REQUIRE(npv == Catch::Approx(6135.87));
}

TEST_CASE("fv_from_coupon", "[fv]") {

    using namespace date;

// Ahorro inicial en el futuro

    double initial = 10000;
    double r = 0.07;
    int maturity = 8;
    auto cal = Schedule(2022_y / 1 / 1, maturity);
    EndCashFlow fv1 = to_future_value(initial, InterestRate(r), cal);
    double aportado1 = initial;
    double presente1 = initial;

    REQUIRE(aportado1 == Catch::Approx(10000));
    REQUIRE(presente1 == Catch::Approx(10000));
    REQUIRE(fv1.cash == Catch::Approx(17181.8617983192));

// Ahorro periodico (anual)
    double cuota;

    cuota = 5000;
//double aportado2 = real_from_coupon(cuota, maturity, Convention::YIELD);
    StartCashFlow presente2 = npv_from_coupon(cuota, InterestRate(r), cal);
    EndCashFlow fv2 = fv_from_coupon(cuota, InterestRate(r), cal);

// REQUIRE(aportado2 == Catch::Approx(40000.0));
    REQUIRE(presente2.cash == Catch::Approx(29856.4925310687));
    REQUIRE(fv2.cash == Catch::Approx(51299.0128451372));

// Ahorro periodico (mensual)

    cuota = 1000;
    int compound_times = 12;
    auto cal2 = Schedule(2022_y / 1 / 1, maturity, Unit::YEAR, 1, Unit::MONTH);
//double aportado3 = real_from_coupon(cuota, maturity, Convention::YIELD, compound_times);
    StartCashFlow presente3 = npv_from_coupon(cuota * compound_times, InterestRate(r, Convention::YIELD, compound_times), cal2);
    EndCashFlow fv3 = fv_from_coupon(cuota * compound_times, InterestRate(r, Convention::YIELD, compound_times), cal2);

    REQUIRE(presente3.cash == Catch::Approx(73347.5686854354));
// REQUIRE(aportado3 == Catch::Approx(96000.0));
    REQUIRE(fv3.cash == Catch::Approx(128198.8210340072));

    double final_value;
    double presente_total;
    double aportado_total = aportado1; // + aportado2 + aportado3;
    presente_total = presente1 + presente2.cash + presente3.cash;
    final_value = fv1.cash + fv2.cash + fv3.cash;
//REQUIRE(coupon_from_real(aportado_total, maturity, Convention::YIELD, 12) == Catch::Approx(1520.8333333333));
//REQUIRE(coupon_from_real(aportado_total, maturity, Convention::YIELD) == Catch::Approx(18250.0));
    REQUIRE(presente_total == Catch::Approx(113204.0612165041));
    REQUIRE(aportado_total == Catch::Approx(10000));
    REQUIRE(final_value == Catch::Approx(196679.6956774635));

    InterestRate r_invest = on_capital(aportado_total, final_value, maturity);
    REQUIRE(r_invest.value == Catch::Approx(0.4511755111));
}

TEST_CASE("fv_from_coupon2", "[fv]")
{
// Ahorro periodico (semanal)
    using namespace date;

    double cuota = 200;
    double frecuencia = 54;
    int maturity = 3.0;
    double r = 0.08;
    auto cal = Schedule(2022_y / 1 / 1, maturity);
    StartCashFlow presente = npv_from_coupon(cuota * frecuencia, InterestRate(r), cal);
//double aportado = real_from_coupon(cuota * frecuencia, maturity, Convention::YIELD);
    EndCashFlow future = fv_from_coupon(cuota * frecuencia, InterestRate(r), cal);

    REQUIRE(presente.cash == Catch::Approx(27832.6474622771));
//REQUIRE(aportado == Catch::Approx(32400.0));
    REQUIRE(future.cash == Catch::Approx(35061.12));
}

TEST_CASE("cagr_interest", "[fv]") {

    double initial = 11000;
    double final = 21000;
    double years = 2;

// past info
    InterestRate ir = on_capital(initial, final, years, Convention::EXPONENTIAL);
    REQUIRE(ir.value * 100 == Catch::Approx(32.3313582463));

    InterestRate ir2 = on_capital(initial, final, years, Convention::YIELD);
    REQUIRE(ir2.value * 100 == Catch::Approx(38.1698559416));

    InterestRate ir3 = on_capital(initial, final, years, Convention::YIELD, 20);
    REQUIRE(ir3.value * 100 == Catch::Approx(32.5941013252));

    InterestRate ir4 = on_capital(initial, final, years, Convention::LINEAR);
    REQUIRE(ir4.value * 100 == Catch::Approx(45.4545454545));

// check IR
    REQUIRE(zero_to_maturity(initial, ir, Maturity(years)) == Catch::Approx(final));
    REQUIRE(zero_to_maturity(initial, ir2, Maturity(years)) == Catch::Approx(final));
    REQUIRE(zero_to_maturity(initial, ir3, Maturity(years)) == Catch::Approx(final));
    REQUIRE(zero_to_maturity(initial, ir4, Maturity(years)) == Catch::Approx(final));

    REQUIRE(maturity_to_zero(final, ir, Maturity(years)) == Catch::Approx(initial));
    REQUIRE(maturity_to_zero(final, ir2, Maturity(years)) == Catch::Approx(initial));
    REQUIRE(maturity_to_zero(final, ir3, Maturity(years)) == Catch::Approx(initial));
    REQUIRE(maturity_to_zero(final, ir4, Maturity(years)) == Catch::Approx(initial));

    REQUIRE(maturity_to_maturity(initial, ir, Maturity::ZERO, Maturity(years)) == Catch::Approx(final));
    REQUIRE(maturity_to_maturity(initial, ir2, Maturity::ZERO, Maturity(years)) == Catch::Approx(final));
    REQUIRE(maturity_to_maturity(initial, ir3, Maturity::ZERO, Maturity(years)) == Catch::Approx(final));
    REQUIRE(maturity_to_maturity(initial, ir4, Maturity::ZERO, Maturity(years)) == Catch::Approx(final));

    REQUIRE(maturity_to_maturity(final, ir, Maturity(years), Maturity::ZERO) == Catch::Approx(initial));
    REQUIRE(maturity_to_maturity(final, ir2, Maturity(years), Maturity::ZERO) == Catch::Approx(initial));
    REQUIRE(maturity_to_maturity(final, ir3, Maturity(years), Maturity::ZERO) == Catch::Approx(initial));
    REQUIRE(maturity_to_maturity(final, ir4, Maturity(years), Maturity::ZERO) == Catch::Approx(initial));

// trading
    initial = 5000;
    double r = 0.10;
    double r_anual = equivalent_rate(r, 12, 1).value;
    years = 3.0;
    REQUIRE(zero_to_maturity(initial,
                             InterestRate(r, Convention::YIELD, 12),
                             Maturity(years)) == \
            Catch::Approx(zero_to_maturity(initial, InterestRate(r_anual), Maturity(years))));
}

TEST_CASE("value & zc", "[fv]") {

    REQUIRE(rate2discount(discount2rate(0.95, 3, Convention::LINEAR), 3, Convention::LINEAR) == Catch::Approx(0.95));
    REQUIRE(discount2rate(rate2discount(0.05, 3, Convention::LINEAR), 3, Convention::LINEAR) == Catch::Approx(0.05));

    REQUIRE(rate2discount(discount2rate(0.95, 3, Convention::LINEAR, 4), 3, Convention::LINEAR) == Catch::Approx(0.95));
    REQUIRE(discount2rate(rate2discount(0.05, 3, Convention::LINEAR), 3, Convention::LINEAR) == Catch::Approx(0.05));

    REQUIRE(rate2discount(discount2rate(0.95, 3, Convention::YIELD), 3, Convention::YIELD) == Catch::Approx(0.95));
    REQUIRE(discount2rate(rate2discount(0.05, 3, Convention::YIELD), 3, Convention::YIELD) == Catch::Approx(0.05));

    REQUIRE(rate2discount(discount2rate(0.95, 3, Convention::YIELD, 4), 3, Convention::YIELD, 4) == Catch::Approx(0.95));
    REQUIRE(discount2rate(rate2discount(0.05, 3, Convention::YIELD, 4), 3, Convention::YIELD, 4) == Catch::Approx(0.05));

    REQUIRE(rate2discount(discount2rate(0.95, 3, Convention::EXPONENTIAL), 3, Convention::EXPONENTIAL) == Catch::Approx(0.95));
    REQUIRE(discount2rate(rate2discount(0.05, 3, Convention::EXPONENTIAL), 3, Convention::EXPONENTIAL) == Catch::Approx(0.05));

    REQUIRE(rate2discount(discount2rate(0.95, 3, Convention::EXPONENTIAL), 3, Convention::EXPONENTIAL) == Catch::Approx(0.95));
    REQUIRE(discount2rate(rate2discount(0.05, 3, Convention::EXPONENTIAL), 3, Convention::EXPONENTIAL) == Catch::Approx(0.05));
}

TEST_CASE("bond_npv2", "[fv]") {

    using namespace date;

    double cash = 17181.8617983192;
    double r = 0.07;
    int maturity = 8;
    auto cal = Schedule(2022_y / 1 / 1, maturity);
    REQUIRE(to_present_value(cash, InterestRate(r), cal).cash == Catch::Approx(10000));

// future value
    double fv = 51299.0128451372;
    REQUIRE(coupon_from_fv(fv, InterestRate(r), cal).cash == Catch::Approx(5000));
    REQUIRE(fv_from_coupon(5000, InterestRate(r), cal).cash == Catch::Approx(fv));

// traer flujos futuros a presente
    StartCashFlow npv = npv_from_coupon(5000, InterestRate(r), cal);
    REQUIRE(npv.cash == Catch::Approx(29856.4925310687));

// Traerme a presente flujos futuros anuales
    REQUIRE(coupon_from_npv(npv.cash, InterestRate(r), cal).cash == Catch::Approx(5000));

    REQUIRE(classic_npv(
            // inversion
            6000,
            // cuota
            500,
            // free risk rate
            InterestRate(0.01),
            // years
            Schedule(2022_y / 1 / 1, 1)) == Catch::Approx(-5504.9504950495));

    double npv1 = classic_npv(1000, 100, InterestRate(-0.1940185202), Schedule(2022_y / 1 / 1, 6));
    REQUIRE(npv1 == Catch::Approx(364.7956282082));

    std::vector<double> cf = { -1000, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };
    InterestRate irr = compute_irr(cf);
    REQUIRE(irr.value == Catch::Approx(0.0292285408));
}

TEST_CASE("real coupon", "[fv]") {

    using namespace date;

    double coupon_netflix = 9.9;
    int maturity = 10;

    auto cal = Schedule(2022_y / 1 / 1, maturity);

    double npv = stock_npv(1000, 0.08, InterestRate(0.03), cal);
    REQUIRE(npv == Catch::Approx(170.6040567355));

    double npv_ = stock_npv(1000, 0.08, InterestRate(0.12), cal);
    REQUIRE(npv_ == Catch::Approx(-904.0356845457));
}

TEST_CASE("tn & te", "[fv]")
{
    double a = 0.05 / 12;

// TASA NOMINAL a TASA EFECTIVA
    double b = equivalent_rate(0.05, 12, 1).value / 12;

// TASA EFECTIVA A TASA NOMINAL
    double c = equivalent_rate(0.05, 1, 12).value / 12;

    double c1 = 1000 * a;
    double c2 = 1000 * b;
    double c3 = 1000 * c;

    REQUIRE(c1 == Catch::Approx(4.1666666667));
    REQUIRE(c2 == Catch::Approx(4.2634914901));
    REQUIRE(c3 == Catch::Approx(4.0741237836));

// 5% reinvirtiendo 1 vez al añao
    REQUIRE(on_capital(1000, 1000 + (c1 * 12)).value == Catch::Approx(0.05));
// 5% reinvirtiendo 12 veces equivalen a 5.1161% reinvirtiendo 1
    REQUIRE(on_capital(1000, 1000 + (c2 * 12)).value == Catch::Approx(0.0511618979));
// 5% reinvirtiendo 1 vez equivalen a 4.888% reinvirtiendo 12
    REQUIRE(on_capital(1000, 1000 + (c3 * 12)).value == Catch::Approx(0.0488894854));

//REQUIRE(tn_2_te(0.05, 12) == Catch::Approx(0.0511618979));
    REQUIRE(equivalent_rate(0.05, 12, 1).value == Catch::Approx(0.0511618979));

//REQUIRE(te_2_tn(0.05, 12) == Catch::Approx(0.0488894854));
    REQUIRE(equivalent_rate(0.05, 1, 12).value == Catch::Approx(0.0488894854));

    REQUIRE(equivalent_rate(0.0488894854, 12, 1).value == Catch::Approx(0.05));
    REQUIRE(equivalent_rate(0.0511618979, 1, 12).value == Catch::Approx(0.05));

    REQUIRE(equivalent_rate(0.01, 365, 1).value == Catch::Approx(0.0100500287));
    REQUIRE(equivalent_rate(0.01, 1, 365).value == Catch::Approx(0.0099504665));

/*
10% mensual con reinversion mensual
*/
    double  fv = zero_to_maturity(1000, InterestRate(0.10 * 12, Convention::YIELD, 12), Maturity::ONE);
    REQUIRE(fv == Catch::Approx(3138.428376721));
    REQUIRE(on_capital(1000, fv).value == Catch::Approx(equivalent_rate(0.10 * 12, 12, 1).value));

/*
10% mensual con reinversion anual = 120%
*/
    double  fv2 = zero_to_maturity(1000, InterestRate(0.10 * 12), Maturity::ONE);
    REQUIRE(fv2 == Catch::Approx(2200.0));
    REQUIRE(on_capital(1000, fv2).value == Catch::Approx(equivalent_rate(0.10 * 12, 1, 1).value));

/*
2% semanal con reinversion semanal = 191.34%
*/
    double  fv3 = zero_to_maturity(1000, InterestRate(0.02 * 54, Convention::YIELD, 54), Maturity::ONE);
    REQUIRE(fv3 == Catch::Approx(2913.4614441403));
    REQUIRE(on_capital(1000, fv3).value == Catch::Approx(InterestRate(0.02 * 54, Convention::YIELD, 54).to_other_interest_rate(Convention::YIELD).value));

/*
2% semanal con reinversion continua = 194.46%
*/
    double fv4 = zero_to_maturity(1000, InterestRate(0.02 * 54, Convention::EXPONENTIAL), Maturity::ONE);
    REQUIRE(fv4 == Catch::Approx(2944.6795510655));
// ¿Como calcular ese CAGR?
    REQUIRE(on_capital(1000, fv4).value == Catch::Approx(InterestRate(0.02 * 54, Convention::EXPONENTIAL).to_other_interest_rate(Convention::YIELD).value));

    REQUIRE(equivalent_rate(0.05, 1, 12) == equivalent_rate(0.05, Convention::YIELD, 1, Convention::YIELD, 12));

    InterestRate other_r = InterestRate(0.2).to_other_interest_rate(Convention::EXPONENTIAL);
    REQUIRE(other_r.value == Catch::Approx(0.1823215568));
}

TEST_CASE("coupon growth", "[fv]")
{
    using namespace date;

    auto cal = Schedule(2022_y / 1 / 1, 5);

// el dividendo no crece
    StartCashFlow npv1 = npv_from_coupon(1000, InterestRate(0.08), cal);
    REQUIRE(npv1.cash == Catch::Approx(3992.7100370781));

// reinvertir anualmente
    StartCashFlow npv2 = npv_from_growth_coupon(1000, InterestRate(0.05), InterestRate(0.08), cal);
    REQUIRE(npv2.cash == Catch::Approx(4379.4737959505));
}

TEST_CASE("coupon growth2", "[fv]")
{
    using namespace date;

    auto cal = Schedule(5, Unit::YEAR, 1, Unit::MONTH);

// npv y fv from growth cupon

    StartCashFlow npv_from_gcoupon = npv_from_growth_coupon(1000, InterestRate(0.05, Convention::YIELD, 12), InterestRate(0.08, Convention::YIELD, 12), cal);
    REQUIRE(npv_from_gcoupon.cash == Catch::Approx(23219.4483321569));

    EndCashFlow fv_from_gcoupon = fv_from_growth_coupon(1000, InterestRate(0.05, Convention::YIELD, 12), InterestRate(0.08, Convention::YIELD, 12), cal);
    REQUIRE(fv_from_gcoupon.cash == Catch::Approx(34593.3954467948));


// cupon from growth cupon

    CouponCashFlow fixed_coupon = coupon_from_growth_coupon(1000,
                                                            InterestRate(0.05, Convention::YIELD, 12),
                                                            InterestRate(0.08, Convention::YIELD, 12),
                                                            cal);
    REQUIRE(fixed_coupon.cash == Catch::Approx(5649.6802745071));

// fv

    CouponCashFlow coupon1 = coupon_from_fv(fv_from_gcoupon.cash, InterestRate(0.08, Convention::YIELD, 12), cal);
    REQUIRE(coupon1.cash == Catch::Approx(fixed_coupon.cash));

    EndCashFlow fv4 = fv_from_coupon(coupon1.cash, InterestRate(0.08, Convention::YIELD, 12), cal);
    REQUIRE(fv4.cash == Catch::Approx(fv_from_gcoupon.cash));

    EndCashFlow fv5 = fv_from_coupon(fixed_coupon.cash, InterestRate(0.08, Convention::YIELD, 12), cal);
    REQUIRE(fv5.cash == Catch::Approx(fv_from_gcoupon.cash));

// npv

    CouponCashFlow coupon2 = coupon_from_npv(npv_from_gcoupon.cash, InterestRate(0.08, Convention::YIELD, 12), cal);
    REQUIRE(coupon2.cash == Catch::Approx(fixed_coupon.cash));

    StartCashFlow npv4 = npv_from_coupon(coupon2.cash, InterestRate(0.08, Convention::YIELD, 12), cal);
    REQUIRE(npv4.cash == Catch::Approx(npv_from_gcoupon.cash));

    StartCashFlow npv5 = npv_from_coupon(fixed_coupon.cash, InterestRate(0.08, Convention::YIELD, 12), cal);
    REQUIRE(npv5.cash == Catch::Approx(npv_from_gcoupon.cash));
}

TEST_CASE("date C++20", "[date]")
{
    using namespace date;
    auto x = 2012_y / 1 / 24;
    auto y = 2013_y / 1 / 8;
    auto diff = (sys_days{ y } - sys_days{ x }).count();
    REQUIRE(diff == Catch::Approx(350));

    auto start_date = day(1) / jan / 2020;
    auto end_date = last / jan / 2030;
    double last_maturity;
    for (auto d = start_date; d < end_date; d += months(1))
    {
// ACT/ACT
        int actual = (sys_days{ jan / day(1) / (d.year() + years(1)) } - sys_days{ jan / day(1) / d.year() }).count();
        double maturity = double((sys_days{ d } - sys_days{ start_date }).count()) / double(actual);
        std::cout << maturity << std::endl;
        std::cout << "for: " << d << " (dia " << d.day() << ")" << ": " << maturity_to_zero(1000,
                                                                                            InterestRate(0.05),
                                                                                            Maturity(maturity)) << std::endl;
        last_maturity = maturity;
    }
    REQUIRE(last_maturity == Catch::Approx(10.0082191781));

    Schedule cal{ start_date, end_date, 3, Unit::MONTH, DayCountConvention::EQUALS };

    std::vector<double> v1, v2;
    for (auto& period : cal.get_forward_periods())
    {
        std::cout << "begin mode = true, pillar: " << period.start.pillar << " - value: " << period.start.value << std::endl;
        v1.push_back(period.start.value);
    }
    for (auto& period : cal.get_forward_periods())
    {
        std::cout << "begin mode = false, pillar: " << period.end.pillar << " - value: " << period.end.value << std::endl;
        v2.push_back(period.end.value);
    }
    std::vector<double> m1, m2;
    int c = 4;
    for (int i = 0; i < 10 * c; ++i)
    {
        std::cout << "value: " << double(i) / c << std::endl;
        m1.push_back(double(i) / c);
    }
    for (int i = 1; i <= 10 * c; ++i)
    {
        std::cout << "value: " << double(i) / c << std::endl;
        m2.push_back(double(i) / c);
    }
    REQUIRE(v1 == m1);
    REQUIRE(v2 == m2);
}

TEST_CASE("forwards1", "[fw]")
{
    using namespace date;

    auto start_date = jan / day(1) / 2020;
    auto end_date = jan / day(1) / 2030;

    Schedule cal{ start_date, end_date, 12, Unit::MONTH, DayCountConvention::EQUALS };
    auto fixings = cal.get_forward_periods();

    InterestRate r(0.08);

    double cash = 1;

    double fwd2 = fixings[0].discount_factor(r).value;
    double fwd3 = fixings[1].discount_factor(r).value;
    double fwd4 = fixings[0].start.to(fixings[2].start).discount_factor(r).value;

    REQUIRE((fwd2) == Catch::Approx(0.9259259259));
    REQUIRE((fwd3) == Catch::Approx(0.9259259259));
    REQUIRE((fwd4) == Catch::Approx(0.9259259259 * 0.9259259259));

    double fwr1 = fixings[0].forward_rate(r).to_other_interest_rate(Convention::YIELD, 12).value / 12.0;
    double fwr2 = fixings[1].forward_rate(r).value;
    double fwr3 = fixings[2].forward_rate(r).value;

    REQUIRE((fwr1) == Catch::Approx(0.0064340301));
    REQUIRE((fwr2) == Catch::Approx(0.08));
    REQUIRE((fwr3) == Catch::Approx(0.08));

    REQUIRE(zero_to_maturity(cash, r, fixings[0].start) == Catch::Approx(1.0));
    REQUIRE(zero_to_maturity(cash, r, fixings[1].start) == Catch::Approx(1.08));
    REQUIRE(zero_to_maturity(cash, r, fixings[2].start) == Catch::Approx(1.1664));
    REQUIRE(to_future_value(cash, r, cal).cash == Catch::Approx(2.1589249972728));

    REQUIRE((1.08 * fwd2) == Catch::Approx(1.0));
    REQUIRE((1.1664 * fwd3) == Catch::Approx(1.08));
    REQUIRE((1.259712 * fwd2 * fwd3) == Catch::Approx(1.08));

    REQUIRE(maturity_to_zero(1.0, r, fixings[0].start) == Catch::Approx(1.0));
    REQUIRE(maturity_to_zero(1.08, r, fixings[1].start) == Catch::Approx(1.0));
    REQUIRE(maturity_to_zero(1.1664, r, fixings[2].start) == Catch::Approx(1.0));
    REQUIRE(to_present_value(2.1589249972728, r, cal).cash == Catch::Approx(1.0));

    double df0 = r.to_discount_factor(fixings[0].start).value;
    double df1 = r.to_discount_factor(fixings[1].start).value;
    double df2 = r.to_discount_factor(fixings[2].start).value;

    REQUIRE(df0 == Catch::Approx(1.0));
    REQUIRE(df1 == Catch::Approx(0.9259259259));
    REQUIRE(df2 == Catch::Approx(0.8573388203));

//
    REQUIRE(fixings[0].start.to(fixings[1].start).discount_factor(r).to_interest_rate(fixings[1].start).value == Catch::Approx(0.08));
    REQUIRE(fixings[0].discount_factor(r).to_interest_rate(fixings[1].start).value == Catch::Approx(0.08));
    REQUIRE(fixings[0].start.to(fixings[1].start).next_discount_factor(r).to_interest_rate(fixings[1].start).value == Catch::Approx(0.1664)); // ??
//
    REQUIRE(fixings[1].discount_factor(r).to_interest_rate(fixings[1].start).value == Catch::Approx(0.08));
    REQUIRE(fixings[0].start.to(fixings[2].start).discount_factor(r).to_interest_rate(fixings[1].start).value == Catch::Approx(0.1664)); // ??
//
    REQUIRE(fixings[2].forward_rate(r).value == fixings[2].start.to(fixings[2].end).forward_rate(r).value);
//
    REQUIRE(Catch::Approx(0.09012224) == fixings[1].start.to(fixings[4].end).forward_rate(r).value);
    REQUIRE(Catch::Approx(0.0938656154) == fixings[0].start.to(fixings[4].end).forward_rate(r).value);
//
    REQUIRE(Catch::Approx(0.7350298528) == fixings[1].start.to(fixings[4].end).discount_factor(r).value);
    REQUIRE(Catch::Approx(0.680583197) == fixings[0].start.to(fixings[4].end).discount_factor(r).value);
//
    REQUIRE(fixings[0].start.to(fixings[1].start).next_discount_factor(r).value == Catch::Approx(fixings[0].start.to(fixings[2].start).discount_factor(r).value));
    REQUIRE(fixings[0].start.to(fixings[2].start).next_discount_factor(r).value == Catch::Approx(fixings[0].start.to(fixings[4].start).discount_factor(r).value));
    REQUIRE(fixings[0].start.to(fixings[3].start).next_discount_factor(r).value == Catch::Approx(fixings[0].start.to(fixings[6].start).discount_factor(r).value));
}

TEST_CASE("POO", "[npv]")
{
    Schedule cal(10);
    InterestRate ir(0.08);

    REQUIRE(StartCashFlow(cal, ir, 1000).to_end_cashflow().cash == Catch::Approx(2158.9249972728));
    REQUIRE(StartCashFlow(cal, ir, 1000).to_coupon().cash == Catch::Approx(149.0294886971));
    REQUIRE(CouponCashFlow(cal, ir, 149.0294886971).to_start_cashflow().cash == Catch::Approx(1000.0));
    REQUIRE(CouponCashFlow(cal, ir, 149.0294886971).to_end_cashflow().cash == Catch::Approx(2158.9249972731));
    REQUIRE(EndCashFlow(cal, ir, 2158.9249972728).to_start_cashflow().cash == Catch::Approx(1000.0));
    REQUIRE(EndCashFlow(cal, ir, 2158.9249972728).to_coupon().cash == Catch::Approx(149.0294886971));
    REQUIRE(CouponCashFlow(cal, ir, 149.0294886971, InterestRate(0.25)).to_start_cashflow().cash == Catch::Approx(2905.0454275324));
    REQUIRE(CouponCashFlow(cal, ir, 149.0294886971, InterestRate(-0.09)).to_start_cashflow().cash == Catch::Approx(718.5193716059));
    REQUIRE(CouponCashFlow(cal, ir, 149.0294886971, InterestRate(0.25)).to_end_cashflow().cash == Catch::Approx(6271.7751917127));
    REQUIRE(CouponCashFlow(cal, ir, 149.0294886971, InterestRate(-0.09)).to_end_cashflow().cash == Catch::Approx(1551.2294323847));

// crecimiento del dividendo
    auto growth = on_capital(0.20, 0.25, 3, Convention::YIELD, Frequency::QUATERLY);
    REQUIRE(growth.value == Catch::Approx(0.0750770605));
    REQUIRE(CouponCashFlow(cal, InterestRate(0.03), 2000, growth).to_end_cashflow().cash == Catch::Approx(32192.5659896183));
}

TEST_CASE("forwards", "[fr]")
{
    using namespace date;

    auto start_date = jan / day(1) / 2020;
    auto end_date = jan / day(1) / 2030;

    Schedule cal{ start_date, end_date, 6, Unit::MONTH, DayCountConvention::EQUALS };

    InterestRate r(0.12);
    auto periods = cal.get_forward_periods();

    for (auto& period : periods)
    {
        std::cout << "start: " << period.start.value << ", end: " << period.end.value << " - fr: " << period.forward_rate(r).value << std::endl;
    }
}

TEST_CASE("csv", "[csv]")
{
/*
rapidcsv::Document doc(R"(D:\dev\deeplearning_dev\dataset\correlation.csv)");

std::vector<float> col = doc.GetColumn<float>("EURGBP_Close");
for(const auto& price : col)
{
    std::cout << price << std::endl;
}
std::cout << "Read " << col.size() << " values." << std::endl;
*/
}

TEST_CASE("spot rate", "[ir]")
{
    InterestRate ir = on_capital(8900, 10000, 5);
    REQUIRE(ir.value == Catch::Approx(0.0235804883));
}

TEST_CASE("curve yield", "[ir]")
{
/*
Tenor	Type	Frequency	Daycount	SwapRate	Date	YearFraction	CumYearFraction	ZeroRate
ON	Deposit	Zero Coupon	ACT360	0.0003	2021-04-21	0.002777778	0.002777778	0.0003
1W	Deposit	Zero Coupon	ACT360	0.000724	2021-04-27	0.016666667	0.019444444	0.000723995
1M	Deposit	Zero Coupon	ACT360	0.001185	2021-05-20	0.063888889	0.083333333	0.001184941
3M	Deposit	Zero Coupon	ACT360	0.0018838	2021-07-20	0.169444444	0.252777778	0.001883352
6M	Deposit	Zero Coupon	ACT360	0.00203	2021-10-20	0.255555556	0.508333333	0.002028953
9M	Deposit	Zero Coupon	ACT360	0.0025	2022-01-20	0.255555556	0.763888889	0.002497616
1Y	Deposit	Zero Coupon	ACT360	0.0028375	2022-04-20	0.25	1.013888889	0.002833426
15M	EuroDollarFuture	Q	ACT360	0.00318125	2022-07-20	0.252777778	1.266666667	0.002909634
18M	EuroDollarFuture	Q	ACT360	0.003525	2022-10-20	0.255555556	1.522222222	0.003025822
21M	EuroDollarFuture	Q	ACT360	0.003583	2023-01-20	0.255555556	1.777777778	0.003117122
2Y	EuroDollarFuture	Q	ACT360	0.003641	2023-04-20	0.25	2.027777778	0.003181505
3Y	EuroDollarFuture	Q	ACT360	0.003797	2024-04-22	1.022222222	3.05	0.007308586
4Y	EuroDollarFuture	Q	ACT360	0.004	2025-04-21	1.011111111	4.061111111	0.009508661
5Y	Swap	S	ACT360	0.00823	2026-04-20	1.011111111	5.072222222	0.008125758
7Y	Swap	S	ACT360	0.01155	2028-04-20	2.030555556	7.102777778	0.011558032
10Y	Swap	S	ACT360	0.01468	2031-04-21	3.044444444	10.14722222	0.013906471
15Y	Swap	S	ACT360	0.01729	2036-04-21	5.075	15.22222222	0.023043104
30Y	Swap	S	ACT360	0.01877	2051-04-20	15.21388889	30.43611111	0.011799239


Leg1			Leg2			Asset
FxdFlt	Fixed		FxdFlt	Float		Leg1PV	139013.01
RateSpread	0.004		RateSpread	0		Leg2PV	-138815.85
Leg1SettlementDate	15-Apr-21		Leg2SettlementDate	15-Apr-21		SwapNPV	197.17
Leg1Frequency	6M		Leg2Frequency	3M
Leg1Horizon	4Y		Leg2Horizon	4Y
Leg1Calendar	NY		Leg2BusinessCalendar	NY
Leg1BusinessDayConvention	Following		Leg2BusinessDayConvention	Following
Leg1DayCountConvention	Thirty360		Leg2DayCountConvention	ACT365
FxdNotional	10000000		FltNotional	-10000000

 */


// Deposits
// https://www.euribor-rates.eu/en/current-euribor-rates/
    double cumyear0 = 0.002777778;
    double swaprate0 = 0.0003;
    double zerorate0 = (1.0 / cumyear0) * log(1.0 + swaprate0 * cumyear0);
    REQUIRE(zerorate0 == Catch::Approx(0.0003));
    REQUIRE(zerorate0 == Catch::Approx(InterestRate(swaprate0, Convention::LINEAR).to_other_interest_rate(cumyear0, Convention::EXPONENTIAL).value));

    double cumyear1 = 0.019444444;
    double swaprate1 = 0.000724;
    double zerorate1 = (1.0 / cumyear1) * log(1.0 + swaprate1 * cumyear1);
    REQUIRE(zerorate1 == Catch::Approx(0.000723995));
    REQUIRE(zerorate1 == Catch::Approx(InterestRate(swaprate1, Convention::LINEAR).to_other_interest_rate(cumyear1, Convention::EXPONENTIAL).value));

    double cumyear_1y = 1.013888889;
    double swaprate_1y = 0.0028375;
    double zerorate_1y = log(1.0 + swaprate_1y * cumyear_1y) / cumyear_1y;
    REQUIRE(zerorate_1y == Catch::Approx(0.002833426));
    REQUIRE(zerorate_1y == Catch::Approx(InterestRate(swaprate_1y, Convention::LINEAR).to_other_interest_rate(cumyear_1y, Convention::EXPONENTIAL).value));

// Futures
// https://www.cmegroup.com/markets/interest-rates/stirs/eurodollar.contractSpecs.html
    double zerorate_2y_prev = 0.003117122;
    double cumyear_2y_prev = 1.777777778;
    double dcf_2y = 0.25;
    double cumyear_2y = 2.027777778;
    double swaprate_2y = 0.003641;
/*
R_eurofut_continuous = 4 × ln[(1 + R_quarterly × YF)]
R_continuous = (R_eurofut_continuous × YF + R_continuous_t-1 × CYF_t-1) / CYF
*/
    double r_continuous_2y = 4.0 * log(1 + swaprate_2y * dcf_2y);
    REQUIRE(r_continuous_2y == Catch::Approx(InterestRate(swaprate_2y, Convention::YIELD, Frequency::QUATERLY).to_other_interest_rate(cumyear_2y, Convention::EXPONENTIAL).value));
    REQUIRE(r_continuous_2y == Catch::Approx(0.0036393439));

// ForwardPeriod per = ForwardPeriod(Maturity(cumyear_2y_prev), Maturity(cumyear_2y));
// InterestRate fr = per.forward_rate(InterestRate(zerorate_2y_prev, Convention::EXPONENTIAL));
// REQUIRE(fr.value == Catch::Approx(0.003181505));

// DiscountFactor df = InterestRate(r_continuous_2y, Convention::EXPONENTIAL).to_discount_factor(dcf_2y) / InterestRate(zerorate_2y_prev, Convention::EXPONENTIAL).to_discount_factor(cumyear_2y_prev);
// REQUIRE(df.to_interest_rate(Convention::EXPONENTIAL).value == Catch::Approx(0.003181505));

// apply transformation
    double w1 = cumyear_2y_prev / cumyear_2y;  // 0.8767
    double w2 = dcf_2y / cumyear_2y;           // 0.1232
    double zerorate_2y = (zerorate_2y_prev * w1 + r_continuous_2y * w2);
    REQUIRE(zerorate_2y == Catch::Approx(0.003181505));

// Swaps
    double dcf_10y = 3.044444444;
    double zerorate_10y_prev = 0.011558032;
    double cumyear_10y_prev = 7.102777778;
    double cumyear_10y = 10.14722222;
    double swaprate_10y = 0.01468;

    double sumproduct = 0.0;
/*
Tenor	Type	Frequency	Daycount	SwapRate	Date	YearFraction	CumYearFraction	ZeroRate
5Y	Swap	S	ACT360	0.00823	2026-04-20	1.011111111	5.072222222	0.008125758
7Y	Swap	S	ACT360	0.01155	2028-04-20	2.030555556	7.102777778	0.011558032
10Y	Swap	S	ACT360	0.01468	2031-04-21	3.044444444	10.14722222	0.013906471
15Y	Swap	S	ACT360	0.01729	2036-04-21	5.075	15.22222222	0.023043104
30Y	Swap	S	ACT360	0.01877	2051-04-20	15.21388889	30.43611111	0.011799239
*/
    sumproduct += 0.00823 * exp(-0.008125758 * 1.011111111);
    sumproduct += 0.01155 * exp(-0.011558032 * 2.030555556);

// 0.014626386588449164
// double r_continuous_10y = InterestRate(swaprate_10y, Convention::YIELD, Frequency::SEMIANNUAL).to_other_interest_rate(cumyear_10y, Convention::EXPONENTIAL).value;

    double zerorate_10y = -1.0 * log((1.0 - sumproduct) / (1.0 + (swaprate_10y / 2.0))) / cumyear_10y;

// REQUIRE(zerorate_10y == Catch::Approx(0.013906471));
// REQUIRE(zerorate_10y == Catch::Approx(InterestRate(swaprate_10y, Convention::LINEAR).to_discount_factor(Maturity(cumyear_10y)).to_interest_rate(cumyear_10y, Convention::EXPONENTIAL).value));

    auto ir2 = InterestRate(swaprate_2y, Convention::YIELD, Frequency::QUATERLY).to_other_interest_rate(cumyear_2y, Convention::EXPONENTIAL);
    w1 = (cumyear_2y - dcf_2y) / cumyear_2y;  // 0.8767
    w2 = dcf_2y / cumyear_2y;           // 0.1232
    zerorate_2y = (zerorate_2y_prev * w1 + ir2.value * w2);
    REQUIRE(zerorate_2y == Catch::Approx(0.003181505));
//term1.append_spot(2.027777778, InterestRate(zerorate_2y, Convention::EXPONENTIAL));

    auto ir3 = InterestRate(0.00318125, Convention::YIELD, Frequency::QUATERLY).to_other_interest_rate(1.266666667, Convention::EXPONENTIAL);
    auto prev_spot = InterestRate(0.002833426, Convention::EXPONENTIAL);
    w1 = 1.013888889 / 1.266666667;
    w2 = 0.252777778 / 1.266666667;
    zerorate_2y = (prev_spot.value * w1 + ir3.value * w2);
    REQUIRE(zerorate_2y == Catch::Approx(0.0029025859));

/*
Tenor	Type	Frequency	Daycount	SwapRate	Date	YearFraction	CumYearFraction	ZeroRate
ON	Deposit	Zero Coupon	ACT360	0.0003	2021-04-21	0.002777778	0.002777778	0.0003
1W	Deposit	Zero Coupon	ACT360	0.000724	2021-04-27	0.016666667	0.019444444	0.000723995
1M	Deposit	Zero Coupon	ACT360	0.001185	2021-05-20	0.063888889	0.083333333	0.001184941
3M	Deposit	Zero Coupon	ACT360	0.0018838	2021-07-20	0.169444444	0.252777778	0.001883352
6M	Deposit	Zero Coupon	ACT360	0.00203	2021-10-20	0.255555556	0.508333333	0.002028953
9M	Deposit	Zero Coupon	ACT360	0.0025	2022-01-20	0.255555556	0.763888889	0.002497616
1Y	Deposit	Zero Coupon	ACT360	0.0028375	2022-04-20	0.25	1.013888889	0.002833426
15M	EuroDollarFuture	Q	ACT360	0.00318125	2022-07-20	0.252777778	1.266666667	0.002909634
18M	EuroDollarFuture	Q	ACT360	0.003525	2022-10-20	0.255555556	1.522222222	0.003025822
21M	EuroDollarFuture	Q	ACT360	0.003583	2023-01-20	0.255555556	1.777777778	0.003117122
2Y	EuroDollarFuture	Q	ACT360	0.003641	2023-04-20	0.25	2.027777778	0.003181505
3Y	EuroDollarFuture	Q	ACT360	0.003797	2024-04-22	1.022222222	3.05	0.007308586
4Y	EuroDollarFuture	Q	ACT360	0.004	2025-04-21	1.011111111	4.061111111	0.009508661
5Y	Swap	S	ACT360	0.00823	2026-04-20	1.011111111	5.072222222	0.008125758
7Y	Swap	S	ACT360	0.01155	2028-04-20	2.030555556	7.102777778	0.011558032
10Y	Swap	S	ACT360	0.01468	2031-04-21	3.044444444	10.14722222	0.013906471
15Y	Swap	S	ACT360	0.01729	2036-04-21	5.075	15.22222222	0.023043104
30Y	Swap	S	ACT360	0.01877	2051-04-20	15.21388889	30.43611111	0.011799239
 */
    TermStructure term1(Convention::EXPONENTIAL);
// deposit
    term1.append_spot(0.002777778, InterestRate(0.0003, Convention::LINEAR));
    term1.append_spot(0.019444444, InterestRate(0.000724, Convention::LINEAR));
    term1.append_spot(0.083333333, InterestRate(0.001185, Convention::LINEAR));
    term1.append_spot(0.252777778, InterestRate(0.0018838, Convention::LINEAR));
    term1.append_spot(0.508333333, InterestRate(0.00203, Convention::LINEAR));
    term1.append_spot(0.763888889, InterestRate(0.0025, Convention::LINEAR));
    term1.append_spot(1.013888889, InterestRate(0.0028375, Convention::LINEAR));
// futures
    term1.append_forward(1.266666667, InterestRate(0.00318125, Convention::YIELD, Frequency::QUATERLY));
    term1.append_forward(1.522222222, InterestRate(0.003525, Convention::YIELD, Frequency::QUATERLY));
    term1.append_forward(1.777777778, InterestRate(0.003583, Convention::YIELD, Frequency::QUATERLY));
    term1.append_forward(2.027777778, InterestRate(0.003641, Convention::YIELD, Frequency::QUATERLY));
    term1.append_forward(3.05, InterestRate(0.003797, Convention::YIELD, Frequency::QUATERLY));
    term1.append_forward(4.061111111, InterestRate(0.004, Convention::YIELD, Frequency::QUATERLY));
// swaps
    term1.append_par(5.072222222, InterestRate(0.00823, Convention::YIELD, Frequency::SEMIANNUAL));
    term1.append_par(7.102777778, InterestRate(0.01155, Convention::YIELD, Frequency::SEMIANNUAL));
    term1.append_par(10.14722222, InterestRate(0.01468, Convention::YIELD, Frequency::SEMIANNUAL));
    term1.append_par(15.22222222, InterestRate(0.01729, Convention::YIELD, Frequency::SEMIANNUAL));
    term1.append_par(30.43611111, InterestRate(0.01877, Convention::YIELD, Frequency::SEMIANNUAL));
// calculate forwards
    term1.build();

// deposits
    REQUIRE(term1.get_spots()[0].value == Catch::Approx(0.0003));
    REQUIRE(term1.get_spots()[1].value == Catch::Approx(0.000724));
    REQUIRE(term1.get_spots()[2].value == Catch::Approx(0.0011849415));
    REQUIRE(term1.get_spots()[3].value == Catch::Approx(0.0018833516));
    REQUIRE(term1.get_spots()[4].value == Catch::Approx(0.0020289533));
    REQUIRE(term1.get_spots()[5].value == Catch::Approx(0.0024976159));
    REQUIRE(term1.get_spots()[6].value == Catch::Approx(0.0028334262));
// futures
    REQUIRE(term1.get_spots()[7].value == Catch::Approx(0.0029025861));
    REQUIRE(term1.get_spots()[8].value == Catch::Approx(0.0030068183));
    REQUIRE(term1.get_spots()[9].value == Catch::Approx(0.0030894139));
    REQUIRE(term1.get_spots()[10].value == Catch::Approx(0.0031572135));
    REQUIRE(term1.get_spots()[11].value == Catch::Approx(0.0033710374));
    REQUIRE(term1.get_spots()[12].value == Catch::Approx(0.0035271351));
// swaps
    REQUIRE(term1.get_spots()[13].value == Catch::Approx(0.0234604466));
    REQUIRE(term1.get_spots()[14].value == Catch::Approx(0.0254132655));
    REQUIRE(term1.get_spots()[15].value == Catch::Approx(0.0243771926));
    REQUIRE(term1.get_spots()[16].value == Catch::Approx(0.020579787));
    REQUIRE(term1.get_spots()[17].value == Catch::Approx(0.0119321278));
}

TEST_CASE("calculate par yield from TermStructure", "[yield]")
{
    TermStructure term;
    term.append_spot(1.0, InterestRate(0.0525));
    term.append_spot(2.0, InterestRate(0.0575));
    term.build();

    double par_yield_to_seach = 0.0;

// high face_value increase precision
    double face_value = 100.0;

    double lr = 100;
    double step = 0.00001;
    bool found = false;
    double search_npv, diff;
    int max_iters = 20000;
    int i = 0;
    while(!found && i<max_iters) {
        search_npv = bond_npv(face_value, par_yield_to_seach, term);
        diff = face_value - search_npv;
        if(abs(diff) < 1e-4)
        {
            found = true;
        }
        else
        {
            if(diff > 0)
                par_yield_to_seach += step * lr;
            else
                par_yield_to_seach -= step * lr;
        }
        i += 1;
    }
    REQUIRE((par_yield_to_seach / face_value) == Catch::Approx(0.05736));

// face value == npv
    double npv = bond_npv(face_value, par_yield_to_seach, term);
    REQUIRE(npv == Catch::Approx(face_value));
}

TEST_CASE("calculate last spot from par yield", "[yield]")
{
    TermStructure term;
    term.append_par(1.0, InterestRate(0.0525));
    term.append_par(2.0, InterestRate(0.05736));
    term.build();

    REQUIRE(term.get_spots()[0].value == Catch::Approx(0.0525));
    REQUIRE(term.get_spots()[1].value == Catch::Approx(0.0575000562));

    double lr = 10;
    double step = 0.00001;
    double max_iter = 20000;
    double face_value = 1000.0;
    double par_yield = 0.05736 * face_value;
    double iter = 0;
    double x = 1.0;
    double npv;
    double diff;
    bool found = false;
    while(!found && iter < max_iter)
    {
        npv = (par_yield / 1.0525) + ((face_value + par_yield) / pow(1.0 + x, 2.0));
        diff = face_value - npv;
        if(abs(diff) < 1e-6)
        {
            found = true;
        }
        else
        {
            if(diff > 0)
                x -= step * lr;
            else
                x += step * lr;
        }
        iter += 1;
    }
    REQUIRE(x == Catch::Approx(0.0576));
}

TEST_CASE("property multiply continuous interest", "[ir exp]")
{
// add forward_periods

    InterestRate r1 = on_capital(1000, 2000, Maturity(3), Convention::EXPONENTIAL);
    InterestRate r2 = on_capital(2000, 3000, Maturity(1), Convention::EXPONENTIAL);

    InterestRate result = on_capital(1000, 3000, Maturity(4), Convention::EXPONENTIAL);

    REQUIRE(r1.value == Catch::Approx(0.2310490602));
    REQUIRE(r2.value == Catch::Approx(0.4054651081));
    REQUIRE(result.value == Catch::Approx(0.2746530722));

    double w1 = 3.0 / 4.0;
    double w2 = 1.0 / 4.0;
    REQUIRE(result.value == Catch::Approx(r1.value * w1 + r2.value * w2));
}

TEST_CASE("property multiply continuous interest with discount factors", "[ir exp]")
{
// add forward_periods

    DiscountFactor r1 = on_capital(1000, 2000, Maturity(3), Convention::EXPONENTIAL).to_discount_factor(Maturity(3));
    DiscountFactor r2 = on_capital(2000, 3000, Maturity(1), Convention::EXPONENTIAL).to_discount_factor(Maturity(1));

    DiscountFactor result = on_capital(1000, 3000, Maturity(4), Convention::EXPONENTIAL).to_discount_factor(Maturity(4));

    REQUIRE(r1.value == Catch::Approx(0.5));
    REQUIRE(r2.value == Catch::Approx(0.6666666667));
    REQUIRE(result.value == Catch::Approx(0.3333333333));

    REQUIRE(result.value == Catch::Approx(r1.value * r2.value));
}

TEST_CASE("property multiply continuous interest 2", "[ir yield]")
{
// add forward_periods
    InterestRate r1 = on_capital(1000, 1100, Maturity(3), Convention::LINEAR).to_other_interest_rate(Maturity(3), Convention::EXPONENTIAL);
    InterestRate r2 = on_capital(1100, 1200, Maturity(1), Convention::YIELD).to_other_interest_rate(Maturity(1), Convention::EXPONENTIAL);

    InterestRate result = on_capital(1000, 1200, Maturity(4), Convention::YIELD).to_other_interest_rate(Maturity(4), Convention::EXPONENTIAL);

    REQUIRE(r1.value == Catch::Approx(0.0317700599));
    REQUIRE(r2.value == Catch::Approx(0.087011377));
    REQUIRE(result.value == Catch::Approx(0.0455803892));

    double w1 = 3.0 / 4.0;
    double w2 = 1.0 / 4.0;
    REQUIRE(result.value == Catch::Approx(r1.value * w1 + r2.value * w2));

    Schedule cal{4};

    for (auto& period : cal.get_forward_periods())
    {
        std::cout << period << std::endl;
    }

    REQUIRE(StartCashFlow(cal, InterestRate(result.value), 1000).to_end_cashflow().cash == Catch::Approx(1195.1700905152));
}

TEST_CASE("bootstraping spot rates from par rates", "[spot rates]")
{
    TermStructure term0;
    term0.append_par(1, InterestRate(0.04));
    term0.append_par(2, InterestRate(0.052));
    term0.append_par(3, InterestRate(0.064));
    term0.build();

    REQUIRE(term0.get_spots()[0].value == Catch::Approx(0.04));
    REQUIRE(term0.get_spots()[1].value == Catch::Approx(0.0523157421));
    REQUIRE(term0.get_spots()[2].value == Catch::Approx(0.0650663354));

    REQUIRE(term0.get_forwards()[0].value == Catch::Approx(0.04));
    REQUIRE(term0.get_forwards()[1].value == Catch::Approx(0.0647773279));
    REQUIRE(term0.get_forwards()[2].value == Catch::Approx(0.0910328794));

    REQUIRE(1 + term0.get_spots()[0].value == Catch::Approx(1 + term0.get_forwards()[0].value));
    REQUIRE(pow(1 + term0.get_spots()[1].value, 2) == Catch::Approx((1 + term0.get_forwards()[0].value) * (1 + term0.get_forwards()[1].value)));
    REQUIRE(pow(1 + term0.get_spots()[2].value, 3) == Catch::Approx((1 + term0.get_forwards()[0].value) * (1 + term0.get_forwards()[1].value) * (1 + term0.get_forwards()[2].value)));

//

    TermStructure term;
    term.append_spot(1, DiscountFactor(99 / 100.0));
    term.append_spot(2, DiscountFactor(97 / 100.0));
    term.append_spot(3, DiscountFactor(95 / 100.0));
    term.append_spot(5, DiscountFactor(90 / 100.0));
    term.append_spot(10, DiscountFactor(73 / 100.0));
    term.build();

    REQUIRE(term.get_spots()[0].value == Catch::Approx(0.0101010101));
    REQUIRE(term.get_spots()[1].value == Catch::Approx(0.0153461651));
    REQUIRE(term.get_spots()[2].value == Catch::Approx(0.0172447682));
    REQUIRE(term.get_spots()[3].value == Catch::Approx(0.0212956876));
    REQUIRE(term.get_spots()[4].value == Catch::Approx(0.0319715249));

    REQUIRE(term.get_forwards()[0].value == Catch::Approx(0.0101010101));
    REQUIRE(term.get_forwards()[1].value == Catch::Approx(0.0206185567));
    REQUIRE(term.get_forwards()[2].value == Catch::Approx(0.0210526316));
    REQUIRE(term.get_forwards()[3].value == Catch::Approx(0.0274023338));
    REQUIRE(term.get_forwards()[4].value == Catch::Approx(0.0427589591));

    REQUIRE(pow(1 + term.get_spots()[1].value, 2) == Catch::Approx((1 + term.get_forwards()[0].value) * (1 + term.get_forwards()[1].value)));

// interpolation discount factor example

    InterestRate ir1(0.05);
    InterestRate ir2(0.07);

    StartCashFlow start_cash1(Schedule(4), ir2, 15000);
    EndCashFlow end_cash1 = start_cash1.to_end_cashflow();

    EndCashFlow end_cash2(Schedule(2), ir1, 20000);
    StartCashFlow start_cash2 = end_cash2.to_start_cashflow();

    DiscountFactor y2(start_cash2.cash / end_cash2.cash);
    DiscountFactor y4(start_cash1.cash / end_cash1.cash);

    TermStructure term2;
    term2.append_spot(2, y2);
    term2.append_spot(3, (y2 + y4) / 2.0);
    term2.append_spot(4, y4);
    term2.build();

    REQUIRE(term2.get_spots()[0].value == Catch::Approx(0.05));
    REQUIRE(term2.get_spots()[1].value == Catch::Approx(0.0619670368));
    REQUIRE(term2.get_spots()[2].value == Catch::Approx(0.07));

    REQUIRE(term2.get_forwards()[0].value == Catch::Approx(0.05));
    REQUIRE(term2.get_forwards()[1].value == Catch::Approx(0.0863118362));
    REQUIRE(term2.get_forwards()[2].value == Catch::Approx(0.0944653107));

//

    TermStructure term3;
    term3.append_spot(2, ir1);
    term3.append_spot(3, (ir1 + ir2) / 2.0);
    term3.append_spot(4, ir2);
    term3.build();

    REQUIRE(term3.get_spots()[0].value == Catch::Approx(0.05));
    REQUIRE(term3.get_spots()[1].value == Catch::Approx(0.06));
    REQUIRE(term3.get_spots()[2].value == Catch::Approx(0.07));

    REQUIRE(term3.get_forwards()[0].value == Catch::Approx(0.05));
    REQUIRE(term3.get_forwards()[1].value == Catch::Approx(0.0802866213));
    REQUIRE(term3.get_forwards()[2].value == Catch::Approx(0.1005696061));

//

    TermStructure term4;
    term4.append_spot(1, InterestRate(0.007));
    term4.append_spot(2, InterestRate(0.012));
    term4.append_spot(3, InterestRate(0.017));
    term4.append_spot(4, InterestRate(0.020));
    term4.build();

    double bond_value = bond_npv(50000, 3000, term4);

    REQUIRE(bond_value == Catch::Approx(57724.2874091592));
//

    TermStructure term5(Convention::YIELD, Frequency::SEMIANNUAL);
    term5.append_spot(0.5, InterestRate(0.006, Convention::YIELD, Frequency::SEMIANNUAL));
    term5.append_spot(1, InterestRate(0.009, Convention::YIELD, Frequency::SEMIANNUAL));
    term5.append_spot(1.5, InterestRate(0.013, Convention::YIELD, Frequency::SEMIANNUAL));
    term5.append_spot(2, InterestRate(0.016, Convention::YIELD, Frequency::SEMIANNUAL));
    term5.build();

    double bond_value2 = bond_npv(100000, 100000*0.04, term5);

    REQUIRE(bond_value2 == Catch::Approx(104737.8902395787));
}

TEST_CASE("start to custom cash flow", "[CF]")
{
    TermStructure term1;
    term1.append_spot(1, InterestRate(0.08, Convention::YIELD, Frequency::ANNUAL));
    term1.append_spot(2, InterestRate(0.08, Convention::YIELD, Frequency::ANNUAL));
    term1.append_spot(3, InterestRate(0.08, Convention::YIELD, Frequency::ANNUAL));
    term1.build();
    StartCashFlow initial(term1, 1000);
    CustomCashFlow cashflow_m2 = initial.to_custom_cashflow(Maturity(3));
    REQUIRE(initial.cash == Catch::Approx(1000));
    REQUIRE(cashflow_m2.cash == Catch::Approx(1259.712));
    REQUIRE(initial.to_custom_cashflow(Maturity(3)).cash == Catch::Approx(initial.to_end_cashflow().cash));
    REQUIRE(initial.to_custom_cashflow(Maturity(2)).cash == Catch::Approx(initial.to_end_cashflow().to_custom_cashflow(Maturity(2)).cash));
}

TEST_CASE("end to custom cash flow", "[CF]")
{
    TermStructure term1;
    term1.append_spot(1, InterestRate(0.08, Convention::YIELD, Frequency::ANNUAL));
    term1.append_spot(2, InterestRate(0.08, Convention::YIELD, Frequency::ANNUAL));
    term1.append_spot(3, InterestRate(0.08, Convention::YIELD, Frequency::ANNUAL));
    term1.build();

    auto initial = term1.make_cashflow(0.0, 1000.0);
    auto final = term1.make_cashflow(3.0, 1259.712);

    REQUIRE(initial.cash == Catch::Approx(1000.0));
    REQUIRE(final.cash == Catch::Approx(1259.712));
    REQUIRE(final.to_custom_cashflow(Maturity(0)).cash == Catch::Approx(final.to_start_cashflow().cash));
    REQUIRE(final.to_custom_cashflow(Maturity(2)).cash == Catch::Approx(final.to_start_cashflow().to_custom_cashflow(Maturity(2)).cash));

// value product

    // Leg -> vector< coupon | start | end | custom > flows;
    Leg product;
    product.flows.emplace_back(initial);
    product.flows.emplace_back(final);
    REQUIRE(product.npv() == Catch::Approx(2000));
}

TEST_CASE("coupon cash flow", "[CF]")
{
    double r = 0.08;

    TermStructure term1;
    term1.append_spot(1, InterestRate(r, Convention::YIELD, Frequency::ANNUAL));
    term1.append_spot(2, InterestRate(r, Convention::YIELD, Frequency::ANNUAL));
    term1.append_spot(3, InterestRate(r, Convention::YIELD, Frequency::ANNUAL));
    term1.build();


    CouponCashFlow coupon(term1, 9.99 * 12);
    REQUIRE(coupon.to_start_cashflow().cash < coupon.to_end_cashflow().cash);  // growth
    REQUIRE(coupon.to_start_cashflow().cash == Catch::Approx(308.9423868313));
    REQUIRE(coupon.to_end_cashflow().cash == Catch::Approx(389.178432));
    REQUIRE(coupon.to_custom_cashflow(Maturity(0)).cash == Catch::Approx(308.9423868313));
    REQUIRE(coupon.to_custom_cashflow(Maturity(1)).cash == Catch::Approx(333.6577777778));
    REQUIRE(coupon.to_custom_cashflow(Maturity(2)).cash == Catch::Approx(360.3504));
    REQUIRE(coupon.to_custom_cashflow(Maturity(3)).cash == Catch::Approx(389.178432));

// POSITIVE RATES = HIGH NPV in long term

    TermStructure term1_neg;
    term1_neg.append_spot(1, InterestRate(-r, Convention::YIELD, Frequency::ANNUAL));
    term1_neg.append_spot(2, InterestRate(-r, Convention::YIELD, Frequency::ANNUAL));
    term1_neg.append_spot(3, InterestRate(-r, Convention::YIELD, Frequency::ANNUAL));
    term1_neg.build();

// NEGATIVES RATES = HIGH NPV in short term

    CouponCashFlow coupon_neg(term1_neg, 9.99 * 12);
    REQUIRE(coupon_neg.to_start_cashflow().cash > coupon_neg.to_end_cashflow().cash);  // decrease
    REQUIRE(coupon_neg.to_start_cashflow().cash == Catch::Approx(425.8907701159));
    REQUIRE(coupon_neg.to_end_cashflow().cash == Catch::Approx(331.636032));
    REQUIRE(coupon_neg.to_custom_cashflow(Maturity(0)).cash == Catch::Approx(425.8907701159));
    REQUIRE(coupon_neg.to_custom_cashflow(Maturity(1)).cash == Catch::Approx(391.8195085066));
    REQUIRE(coupon_neg.to_custom_cashflow(Maturity(2)).cash == Catch::Approx(360.4739478261));
    REQUIRE(coupon_neg.to_custom_cashflow(Maturity(3)).cash == Catch::Approx(331.636032));

    TermStructure term1_zero;
    term1_zero.append_spot(1, InterestRate(0.0, Convention::YIELD, Frequency::ANNUAL));
    term1_zero.build();

    CouponCashFlow coupon_zero(term1_zero, 9.99 * 12);
    REQUIRE(coupon_zero.to_start_cashflow().cash == Catch::Approx(coupon_zero.to_end_cashflow().cash));
    REQUIRE(coupon_zero.to_start_cashflow().cash == Catch::Approx(119.88));
    REQUIRE(coupon_zero.to_end_cashflow().cash == Catch::Approx(119.88));
    REQUIRE(coupon_zero.to_custom_cashflow(Maturity(0)).cash == Catch::Approx(119.88));
    REQUIRE(coupon_zero.to_custom_cashflow(Maturity(1)).cash == Catch::Approx(119.88));
    REQUIRE(coupon_zero.to_custom_cashflow(Maturity(2)).cash == Catch::Approx(119.88));
    REQUIRE(coupon_zero.to_custom_cashflow(Maturity(3)).cash == Catch::Approx(119.88));
}

TEST_CASE("convert interest rate and forwards", "[ir]")
{
    InterestRate spot(0.05);

    TermStructure term1;
    term1.append_spot(1, spot);
    term1.build();

// forward empieza en 2 y termina en 4
    InterestRate spot_exp = spot.to_other_interest_rate(4, Convention::EXPONENTIAL);
    InterestRate forward = spot.to_other_interest_rate(4, Convention::EXPONENTIAL);

    REQUIRE(spot.value == Catch::Approx(0.05));
    REQUIRE(spot_exp.value == Catch::Approx(0.0487901642));
    REQUIRE(forward.value == Catch::Approx(0.0487901642));

    REQUIRE(spot.to_discount_factor(Maturity::ZERO).value == Catch::Approx(1.0));
    REQUIRE(spot_exp.to_discount_factor(Maturity::ZERO).value == Catch::Approx(1.0));
    REQUIRE(forward.to_discount_factor(Maturity(2)).value == Catch::Approx(0.9070294785));
    REQUIRE(forward.to_discount_factor(Maturity(4)).value == Catch::Approx(0.8227024748));

// Nos llevamos el dinero a 2
// Nos llevamos el dinero a 4
// Nos llevamos el dinero de 4 a 2

    auto cf0 = term1.make_cashflow(Maturity::ZERO, 1000);
    auto cf2 = cf0.to_custom_cashflow(Maturity(2));
    auto cf4 = cf0.to_custom_cashflow(Maturity(4));
    auto cf4_to_2 = cf4.to_custom_cashflow(Maturity(2));
    REQUIRE(cf2.cash == Catch::Approx(1102.5));
    REQUIRE(cf4.cash == Catch::Approx(1215.50625));
    REQUIRE(cf4_to_2.cash == Catch::Approx(1102.5));

    REQUIRE(1215.50625 * 0.9070294785 == Catch::Approx(1102.5));

    REQUIRE(Maturity(4).to(2).discount_factor(spot).value == Catch::Approx(1215.50625 / 1102.5));

    REQUIRE(on_capital(1102.5, 1215.50625, Maturity(2), Convention::EXPONENTIAL).value == Catch::Approx(0.0487901642));

    ForwardPeriod period(Maturity(2), Maturity(4));
    InterestRate irf = period.forward_rate(spot);

    REQUIRE(maturity_to_maturity(1102.5, spot, Maturity(2), Maturity(4)) == Catch::Approx(1215.50625));
    REQUIRE(maturity_to_maturity(1215.50625, spot, Maturity(4), Maturity(2)) == Catch::Approx(1102.5));

    REQUIRE(irf.value == Catch::Approx(0.05125));
    REQUIRE(irf.to_other_interest_rate(Maturity(2), Convention::EXPONENTIAL).value == Catch::Approx(0.0499799323));
    REQUIRE(irf.to_other_interest_rate(Maturity(4), Convention::EXPONENTIAL).value == Catch::Approx(0.0499799323));

// Solution
    REQUIRE(DiscountFactor(1102.5 / 1215.50625).to_interest_rate(Maturity(2)).value == Catch::Approx(0.05));
    REQUIRE(DiscountFactor(1102.5 / 1215.50625).to_interest_rate(Maturity(4)).value == Catch::Approx(0.0246950766));
    REQUIRE(DiscountFactor(1102.5 / 1215.50625).to_interest_rate(Maturity(2), Convention::EXPONENTIAL).value == Catch::Approx(0.0487901642));
    REQUIRE(DiscountFactor(1102.5 / 1215.50625).to_interest_rate(Maturity(4), Convention::EXPONENTIAL).value == Catch::Approx(0.0243950821));
}
