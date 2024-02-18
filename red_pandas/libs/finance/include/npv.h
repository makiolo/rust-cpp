//
// Created by n424613 on 21/10/2022.
//

#ifndef RED_PANDAS_PROJECT_NPV_H
#define RED_PANDAS_PROJECT_NPV_H

// https://www.linkedin.com/pulse/python-bootstrapping-zero-curve-sheikh-pancham#:~:text=The%20objective%20of%20bootstrapping%20is,the%20end%20of%20its%20term.
#include <cmath>
#include <vector>
#include <chrono>
// header only date: https://raw.githubusercontent.com/HowardHinnant/date/master/include/date/date.h
// date algorithms: http://howardhinnant.github.io/date_algorithms.html
// date doc: https://howardhinnant.github.io/date.html
#include "date.h"
//#include "rapidcsv.h"
#include "portfolio.h"

// AoS
struct packed {
    double close;
    double open;
    double high;
    double low;
    double volume;
};

// SoA
struct packed_soa {
    double close[100]; // use std::vector ?
    double open[100];
    double high[100];
    double low[100];
    double volume[100];
};

struct packed_helper {

    // AoS
    packed prices[100];

    // SoA
    packed_soa prices_soa;

    double* transpose_close() {
        long i = 0;
        for (const auto& p : prices)
        {
            prices_soa.close[i] = p.close;
            ++i;
        }
        return prices_soa.close;
    }
};



namespace qs {

    class ForwardPeriod;
    class DiscountFactor;
    class InterestRate;
    class CashFlow;
    class StartCashFlow;
    class EndCashFlow;
    class CouponCashFlow;
    class CustomCashFlow;
    class Maturity;
    class Schedule;
    class TermStructure;

    enum Convention
    {
        LINEAR,
        YIELD,
        EXPONENTIAL,
    };

    enum Frequency
    {
        ANNUAL = 1,
        SEMIANNUAL = 2,
        QUATERLY = 4,
        MONTHLY = 12,
    };

    enum Unit
    {
        YEAR = 12,
        SEMESTER = 6,
        TRIMESTER = 3,
        MONTH = 1,
    };

    enum DayCountConvention
    {
        ACT_ACT,
        ACT_360,
        ACT_365,
        EQUALS,
    };

    // convert value and zc
    double discount2rate(double df, double maturity, Convention conv = Convention::YIELD, int compound_times = Frequency::ANNUAL);
    double rate2discount(double zc, double maturity, Convention conv = Convention::YIELD, int compound_times = Frequency::ANNUAL);
    InterestRate equivalent_rate(const Maturity& maturity, double rate, Convention conv, int compound_times, Convention other_convention = Convention::YIELD, int other_compound_times = Frequency::ANNUAL);
    InterestRate equivalent_rate(double rate, Convention conv, int compound_times, Convention other_convention = Convention::YIELD, int other_compound_times = Frequency::ANNUAL);
    InterestRate equivalent_rate(double rate, int compound_times, int other_compound_times = Frequency::ANNUAL);

    // one cash flow
    double maturity_to_zero(double cash, const InterestRate& r, const Maturity& maturity);
    double zero_to_maturity(double cash, const InterestRate& r, const Maturity& maturity);
    double maturity_to_maturity(double cash, const InterestRate& r, const Maturity& maturityA, const Maturity& maturityB);
    double maturity_to_maturity(double cash, const InterestRate& rA, const InterestRate& rB, const Maturity& maturityA, const Maturity& maturityB);
    StartCashFlow to_present_value(double cashflow, const InterestRate& r, const Schedule& cal);
    EndCashFlow to_future_value(double cashflow, const InterestRate& r, const Schedule& cal);
    StartCashFlow to_present_value(const EndCashFlow& cashflow, const TermStructure& term);
    EndCashFlow to_future_value(const StartCashFlow& cashflow, const TermStructure& term);

    // fv - coupon - fv
    StartCashFlow npv_from_coupon(double coupon, const InterestRate& interest_rate, const Schedule& cal);
    CouponCashFlow coupon_from_npv(double npv, const InterestRate& interest_rate, const Schedule& cal);
    CouponCashFlow coupon_from_npv(double npv, const TermStructure& term);
    CouponCashFlow coupon_from_fv(double fv, const InterestRate& interest_rate, const Schedule& cal);
    CouponCashFlow coupon_from_fv(double fv, const TermStructure& term);
    EndCashFlow fv_from_coupon(double coupon, const InterestRate& interest_rate, const Schedule& cal);

    // growth_coupon
    StartCashFlow npv_from_growth_coupon(double coupon, const InterestRate& growth_rate, const InterestRate& interest_rate, const Schedule& cal);
    EndCashFlow fv_from_growth_coupon(double coupon, const InterestRate& growth_rate, const InterestRate& interest_rate, const Schedule& cal);
    CouponCashFlow coupon_from_growth_coupon(double coupon, const InterestRate& growth_rate, const InterestRate& interest_rate, const Schedule& cal);
    StartCashFlow npv_from_growth_coupon(double coupon, const InterestRate& growth_rate, const TermStructure& term);
    EndCashFlow fv_from_growth_coupon(double coupon, const InterestRate& growth_rate, const TermStructure& term);
    CouponCashFlow coupon_from_growth_coupon(double coupon, const InterestRate& growth_rate, const TermStructure& term);

    /*
    growth_coupon_from_npv
    growth_coupon_from_coupon
    grouth_coupon_from_fv
    */
    InterestRate compute_irr(const std::vector<double>& cf, Convention convention = Convention::YIELD, int compound_times = Frequency::ANNUAL);

    // value products
    double classic_npv(double investment, double coupon, const InterestRate& interest_rate, const Schedule& cal);
    double bond_npv(double face_value, double coupon, const InterestRate& interest_rate, const Schedule& cal);
    double bond_npv(double face_value, double coupon, const TermStructure& term);
    double stock_npv(double investment, double dividend, const InterestRate& interest_rate, const Schedule& cal);

    // utils
    double inverse_lerp(double min_value, double max_value, double v);
    double lerp(double min_value, double max_value, double t);

    class Maturity
    {
    public:
        Maturity(double value_)
                : value(value_)
                , has_pillar(false)
        {
            ;
        }

        explicit Maturity(const date::year_month_day& pillar_, double value_)
                : pillar(pillar_)
                , value(value_)
                , has_pillar(true)
        {
            ;
        }

        ForwardPeriod to(const Maturity& target) const;
        DiscountFactor get_discount_factor(const InterestRate& ir) const;

        bool operator>(const Maturity& rhs) const
        {
            return value > rhs.value;
        }

        bool operator<(const Maturity& rhs) const
        {
            return value < rhs.value;
        }

        bool operator>=(const Maturity& rhs) const
        {
            return value >= rhs.value;
        }

        bool operator<=(const Maturity& rhs) const
        {
            return value <= rhs.value;
        }

        bool has_pillar;
        date::year_month_day pillar;
        double value;

        static const Maturity ZERO;
        static const Maturity ONE;
    };


    // calcular value
    InterestRate on_capital(double initial_value, double final_value, const Maturity& maturity = Maturity::ONE, Convention convention = Convention::YIELD, int compound_times = Frequency::ANNUAL);


    class ForwardPeriod
    {
    public:
        ForwardPeriod(const Maturity& start_, const Maturity& end_)
                : start(start_)
                , end(end_)
        {

        }

        double duration() const
        {
            return end.value - start.value;
        }

        bool inside(const Maturity& mat) const
        {
            return start.value < mat.value && mat.value <= end.value;
        }

        DiscountFactor discount_factor(const InterestRate& ir) const;
        DiscountFactor discount_factor(const InterestRate& ir_start, const InterestRate& ir_end) const;
        InterestRate forward_rate(const InterestRate& ir) const;
        InterestRate forward_rate(const InterestRate& ir_start, const InterestRate& ir_end) const;
        DiscountFactor next_discount_factor(const InterestRate& ir) const;

        Maturity start;
        Maturity end;
    };

    class ZeroPeriod : public ForwardPeriod
    {
    public:
        explicit ZeroPeriod(const Maturity& end)
                : ForwardPeriod(Maturity::ZERO, end)
        {

        }
    };

    std::ostream& operator<<(std::ostream& out, const Maturity& obj);
    std::ostream& operator<<(std::ostream& out, const ForwardPeriod& obj);

    // struct Calendar
    // {
        // TODO:
    // };

    struct Schedule
    {
        explicit Schedule()
        {
            // dummy constructor
        }

        explicit Schedule(const date::year_month_day& start_date_, const date::year_month_day& end_date_, int tenor_ = 1, Unit tenor_step_ = Unit::YEAR, DayCountConvention dc_convention_ = DayCountConvention::EQUALS)
                : start_date(start_date_)
                , end_date(end_date_)
                , tenor(tenor_)
                , tenor_step(tenor_step_)
                , dc_convention(dc_convention_)
        {
            build();
        }

        explicit Schedule(const date::year_month_day& start_date_, int duration_years, Unit duration_unit = Unit::YEAR, int tenor_ = 1, Unit tenor_step_ = Unit::YEAR, DayCountConvention dc_convention_ = DayCountConvention::EQUALS)
                : start_date(start_date_)
                , end_date(start_date_ + date::months(inc_tenor(duration_years, duration_unit)))
                , tenor(tenor_)
                , tenor_step(tenor_step_)
                , dc_convention(dc_convention_)
        {
            build();
        }

        explicit Schedule(int duration_years, Unit duration_unit = Unit::YEAR, int tenor_ = 1, Unit tenor_step_ = Unit::YEAR, DayCountConvention dc_convention_ = DayCountConvention::EQUALS)
                : start_date(date::jan / date::day(1) / 2020)
                , end_date((date::jan / date::day(1) / 2020) + date::months(inc_tenor(duration_years, duration_unit)))
                , tenor(tenor_)
                , tenor_step(tenor_step_)
                , dc_convention(dc_convention_)
        {
            build();
        }

        explicit Schedule(const std::vector<Maturity>& maturities)
        {
            Maturity prev = Maturity::ZERO;

            for (const auto& mat : maturities)
            {
                forward_periods.emplace_back(prev, mat);
                spot_periods.emplace_back(mat);
                prev = mat;
            }
        }

        const std::vector<ForwardPeriod>& get_forward_periods() const
        {
            return forward_periods;
        }

        const std::vector<ZeroPeriod>& get_spot_periods() const
        {
            return spot_periods;
        }

        ForwardPeriod get_first_period() const
        {
            auto mats = forward_periods;
            return mats.front();
        }

        ForwardPeriod get_last_period() const
        {
            auto mats = forward_periods;
            return mats.back();
        }

        std::vector<InterestRate> spot_to_forward(const std::vector<InterestRate>& spots, Convention conv = Convention::YIELD, int compound_times = Frequency::ANNUAL) const;

    protected:

        int inc_tenor(int amount, Unit freq) const
        {
            switch (freq)
            {
                case Unit::YEAR:
                {
                    return amount * 12;
                    break;
                }
                case Unit::SEMESTER:
                {
                    return amount * 6;
                    break;
                }
                case Unit::TRIMESTER:
                {
                    return amount * 3;
                    break;
                }
                case Unit::MONTH:
                default:
                {
                    return amount;
                    break;
                }
            }
        }

        void build()
        {
            using namespace date;

            std::vector<ForwardPeriod> data;
            auto pillar_day = start_date;
            int i = 0;
            double prev = 0.0, count = 0.0;

            // jump to 1
            pillar_day += months(inc_tenor(tenor, tenor_step));
            i++;

            while (pillar_day <= end_date)
            {
                prev = count;
                switch (dc_convention)
                {
                    case DayCountConvention::ACT_ACT:
                    {
                        double m = double((sys_days{ jan / day(1) / (pillar_day.year() + years(1)) } - sys_days{ jan / day(1) / pillar_day.year() }).count());
                        count = double((sys_days{ pillar_day } - sys_days{ start_date }).count()) / m;
                        break;
                    }
                    case DayCountConvention::ACT_365:
                    {
                        count = double((sys_days{ pillar_day } - sys_days{ start_date }).count()) / 365.0;
                        break;
                    }
                    case DayCountConvention::EQUALS:
                    {
                        switch (tenor_step)
                        {
                            case Unit::YEAR:
                            {
                                count = double(i) * tenor;
                                break;
                            }
                            case Unit::SEMESTER:
                            {
                                count = (double(i) * tenor) / 2.0;
                                break;
                            }
                            case Unit::TRIMESTER:
                            {
                                count = (double(i) * tenor) / 4.0;
                                break;
                            }
                            case Unit::MONTH:
                            default:
                            {
                                count = (double(i) * tenor) / 12.0;
                                break;
                            }
                        }
                        break;
                    }
                    case DayCountConvention::ACT_360:
                    default:
                    {
                        count = double((sys_days{ pillar_day } - sys_days{ start_date }).count()) / 360.0;
                        break;
                    }
                }
                auto start = Maturity(pillar_day, prev);
                pillar_day += months(inc_tenor(tenor, tenor_step));
                auto end = Maturity(pillar_day, count);
                forward_periods.emplace_back(start, end);
                spot_periods.emplace_back(end);
                i += 1;
            }
        }

    protected:
        std::vector<ForwardPeriod> forward_periods;
        std::vector<ZeroPeriod> spot_periods;

    private:
        date::year_month_day start_date;
        date::year_month_day end_date;
        int tenor;
        Unit tenor_step;
        DayCountConvention dc_convention;
    };

    class DiscountFactor
    {
    public:
        DiscountFactor(double value_)
                : value(value_)
        {
            ;
        }

        DiscountFactor(const DiscountFactor& other)
        {
            value = other.value;
        }

        DiscountFactor(DiscountFactor&& other) noexcept
        {
            value = other.value;
        }

        InterestRate to_interest_rate(const Maturity& maturity = Maturity::ONE, Convention convention_ = Convention::YIELD, int compound_times_ = Frequency::ANNUAL) const;

        friend DiscountFactor operator*(const DiscountFactor& l, const DiscountFactor& r)
        {
            return { l.value * r.value };
        }

        friend DiscountFactor operator/(const DiscountFactor& l, const DiscountFactor& r)
        {
            return { l.value / r.value };
        }

        friend DiscountFactor operator*(const DiscountFactor& l, double r)
        {
            return { l.value * r };
        }

        friend DiscountFactor operator/(const DiscountFactor& l, double r)
        {
            return { l.value / r };
        }

        friend DiscountFactor operator+(const DiscountFactor& l, const DiscountFactor& r)
        {
            return { l.value + r.value };
        }

        friend DiscountFactor operator-(const DiscountFactor& l, const DiscountFactor& r)
        {
            return { l.value - r.value };
        }

    public:
        double value;
    };

    class InterestRate
    {
    public:
        explicit InterestRate(double value_, Convention convention_ = Convention::YIELD, int compound_times_ = Frequency::ANNUAL)
                : value(value_)
                , conv(convention_)
                , c(compound_times_)
        {
            ;
        }

        InterestRate(const InterestRate& other)
        {
            value = other.value;
            c = other.c;
            conv = other.conv;
        }

        InterestRate(InterestRate&& other) noexcept
        {
            value = other.value;
            c = other.c;
            conv = other.conv;
        }

        InterestRate& operator=(const InterestRate& other)
        {
            value = other.value;
            c = other.c;
            conv = other.conv;
            return *this;
        }

        InterestRate& operator=(InterestRate&& other) noexcept
        {
            value = other.value;
            c = other.c;
            conv = other.conv;
            return *this;
        }

        bool operator==(const InterestRate& rhs) const
        {
            return         value == rhs.value &&
                           c == rhs.c &&
                           conv == rhs.conv;
        }

        std::vector<DiscountFactor> get_discount_factor_start(const Schedule& cal) const
        {
            std::vector<DiscountFactor> dfs;
            for (auto& period : cal.get_forward_periods())
            {
                dfs.push_back(period.start.get_discount_factor(*this));
            }
            return dfs;
        }

        std::vector<DiscountFactor> get_discount_factors_end(const Schedule& cal) const
        {
            std::vector<DiscountFactor> dfs;
            for (auto& period : cal.get_forward_periods())
            {
                dfs.push_back(period.end.get_discount_factor(*this));
            }
            return dfs;
        }

        DiscountFactor to_discount_factor(const Maturity& maturity) const;
        InterestRate to_other_interest_rate(Convention other_convention, int other_compound_times = Frequency::ANNUAL) const;
        InterestRate to_other_interest_rate(const Maturity& maturity, Convention other_convention, int other_compound_times = Frequency::ANNUAL) const;

        friend InterestRate operator*(const InterestRate& l, double r)
        {
            return InterestRate(l.value * r, l.conv, l.c);
        }

        friend InterestRate operator/(const InterestRate& l, double r)
        {
            return InterestRate(l.value / r, l.conv, l.c);
        }

        friend InterestRate operator+(const InterestRate& l, const InterestRate& r)
        {
            // TODO: Is owner?
            assert(l.conv == r.conv);
            assert(l.c == r.c);
            return InterestRate(l.value + r.value, l.conv, l.c);
        }

        friend InterestRate operator-(const InterestRate& l, const InterestRate& r)
        {
            // TODO: Is owner?
            assert(l.conv == r.conv);
            assert(l.c == r.c);
            return InterestRate(l.value - r.value, l.conv, l.c);
        }

    public:
        double value;  // annual rate
        int c;  // reinversions each year
        Convention conv;  // convention

        static const InterestRate ZERO;
    };

    //////////////////////////////////////////////////////////////

    // class LegGenerator
    // {
    // public:
        // schedule each 1 month -> next maturity
        // end date
        /*
        swap (
           [ leg ( schedule="1bd f",
                  start_date = "Spot",
                  basis = "ACT_360",
                  end_date = "1bd",
                  capital_payment_mode = "TTT" )
           ]
        )
        */
    // };

    // class ProductGenerator
    // {
    //    std::vector<LegGenerator> leg_generators;
    // };

    class Leg  // like Leg
    {
    public:
        double npv() const;
        void add(const std::shared_ptr<const CustomCashFlow>& flow);
        void add(const CustomCashFlow& flow);
    public:
        std::vector< std::shared_ptr<const CustomCashFlow> > flows;

        // CounterParty from
        // CounterParty to
    };

    class Product
    {
    public:
        void add(const Leg& leg)
        {
            legs.emplace_back(leg);
        }

        double npv() const
        {
            double npv = 0.0;
            for(const auto& leg : legs)
            {
                npv += leg.npv();
            }
            return npv;
        }
    public:
        std::vector< Leg > legs;
        // Product open linked to Product close
        // and viceversa
        // Product* product_linked;
    };

    class TermStructure
    {
    public:
        explicit TermStructure(Convention convention_ = Convention::YIELD, int compound_times_ = Frequency::ANNUAL)
                : built(false)
                , _convention(convention_)
                , _compound_times(compound_times_)
        {

        }

        explicit TermStructure(const Schedule& cal, const InterestRate& interest_rate, Convention convention_ = Convention::YIELD, int compound_times_ = Frequency::ANNUAL)
                : built(false)
                , _convention(convention_)
                , _compound_times(compound_times_)
        {
            for(const auto& period : cal.get_forward_periods())
            {
                append_spot(period.end, interest_rate);
            }
            build();
        }

        explicit TermStructure(const Schedule& cal, const DiscountFactor& discount_factor)
                : built(false)
        {
            for(const auto& period : cal.get_forward_periods())
            {
                append_spot(period.end, discount_factor);
            }
            build();
        }

        void append_spot(const Maturity& maturity, const DiscountFactor& discount_factor)
        {
            _maturities.push_back(maturity);
            _interest_rates.push_back(discount_factor.to_interest_rate(maturity, _convention, _compound_times));
        }

        void append_spot(const Maturity& maturity, const InterestRate& ir)
        {
            _maturities.push_back(maturity);
            if(ir.conv == _convention && ir.c == _compound_times)
            {
                _interest_rates.push_back(ir);
            }
            else
            {
                _interest_rates.push_back(
                        ir.to_other_interest_rate(maturity, _convention, _compound_times));
            }
        }

        void append_forward(const Maturity& maturity, const InterestRate& ir)
        {
            if(_maturities.empty())
            {
                append_spot(maturity, ir);
                return;
            }
            Maturity last_mat = get_last_maturity();
            InterestRate last_ir = get_last_interest_rate();
            InterestRate ir_exp = ir.to_other_interest_rate(maturity, Convention::EXPONENTIAL);
            double w1 = last_mat.value / maturity.value;
            double w2 = (maturity.value - last_mat.value) / maturity.value;
            double spot_rate = last_ir.value * w1 + ir_exp.value * w2;
            append_spot(maturity, InterestRate(spot_rate, ir_exp.conv));
        }

        void append_par(const Maturity& maturity, const InterestRate& ir)
        {
            if(_maturities.empty())
            {
                append_spot(maturity, ir);
                return;
            }

            double accum = 0.0;
            for (int i = 0; i < _maturities.size(); ++i)
            {
                accum += maturity_to_zero(ir.value, _interest_rates[i], Maturity(i + 1));
            }
            InterestRate spotN = DiscountFactor((1.0 - accum) / (1.0 + ir.value)).to_interest_rate(maturity, _convention, _compound_times);
            append_spot(maturity, spotN);
        }

        void build()
        {
            if(built)
                throw std::runtime_error("TermStructure already built.");

            _cal = Schedule(_maturities);
            _forward_rates = _cal.spot_to_forward(_interest_rates, _convention, _compound_times);
            built = true;
        }

        std::vector<DiscountFactor> get_discount_factor_start() const
        {
            assert(built == true);

            std::vector<DiscountFactor> dfs;
            int i = 0;
            for (auto& period : _cal.get_forward_periods())
            {
                dfs.push_back(period.start.get_discount_factor(_interest_rates[i]));
                i += 1;
            }
            return dfs;
        }

        std::vector<DiscountFactor> get_discount_factors_end() const
        {
            assert(built == true);

            std::vector<DiscountFactor> dfs;
            int i = 0;
            for (auto& period : _cal.get_forward_periods())
            {
                dfs.push_back(period.end.get_discount_factor(_interest_rates[i]));
                i += 1;
            }
            return dfs;
        }

        // calcular interest rate entre 2 maturities (interpolados?)
        // obtener discount factor de 1 maturity
        // obtener interest rate de 1 maturity

        // return interpolated InterestRate
        InterestRate get_interpolated_interest_rate(const Maturity& maturity) const
        {
            auto first_mat = get_first_maturity();
            if(maturity < first_mat)
            {
                return get_first_interest_rate();
            }
            auto last_mat = get_last_maturity();
            if(maturity > last_mat)
            {
                return get_last_interest_rate();
            }

            int i = 0;
            ForwardPeriod const* candidate = nullptr;
            InterestRate const* ir_prev = nullptr;
            for (const auto& period : _cal.get_forward_periods())
            {
                if(period.inside(maturity))
                {
                    ir_prev = &_interest_rates[i];
                    candidate = &period;
                }
                if(candidate != nullptr)
                {
                    double interpolator = inverse_lerp(period.start.value, period.end.value, maturity.value);
                    InterestRate const& a = *ir_prev;
                    InterestRate const& b = _interest_rates[i];

                    // TODO: different InterestRates ?
                    assert(a.conv == b.conv);
                    assert(a.c == b.c);

                    double interpolated = lerp(a.value, b.value, interpolator);
                    return InterestRate(interpolated, a.conv, a.c);
                }
                i += 1;
            }

            return InterestRate::ZERO;
        }

        CustomCashFlow make_cashflow(const Maturity& maturity, double cash);

        /*
         * product_id create_cashflow_founder(a, b, maturity, cash)
         * void create_cashflow_continuation(product_id, a, b, maturity, cash)
         * void create_cashflow_close(product_id, a, b, maturity, cash)
         * Product get_product(product_id)
         * vector<Procuct> get_products()
         */

        std::vector< InterestRate >& get_spots()
        {
            return _interest_rates;
        }

        const std::vector< InterestRate >& get_spots() const
        {
            return _interest_rates;
        }

        std::vector< InterestRate >& get_forwards()
        {
            return _forward_rates;
        }

        const std::vector< InterestRate >& get_forwards() const
        {
            return _forward_rates;
        }

        Maturity get_first_maturity() const
        {
            auto mats = _maturities;
            return mats.front();
        }

        InterestRate get_first_interest_rate() const
        {
            auto irs = _interest_rates;
            return irs.front();
        }

        Maturity get_last_maturity() const
        {
            auto mats = _maturities;
            return mats.back();
        }

        InterestRate get_last_interest_rate() const
        {
            auto irs = _interest_rates;
            return irs.back();
        }

    public:
        Convention _convention;
        int _compound_times;
    protected:
        bool built;
        Schedule _cal;
        std::vector< Maturity > _maturities;
        std::vector< InterestRate > _interest_rates;
        std::vector< InterestRate > _forward_rates;
        pf::BlockChain blockchain;

    };

    class CashFlow
    {
    public:
        explicit CashFlow(const Schedule& cal_, const InterestRate& ir_, double cash_)
                : term(cal_, ir_)
                , cash(cash_)
        {
            ;
        }

        explicit CashFlow(const TermStructure& term_, double cash_)
                : term(term_)
                , cash(cash_)
        {
            ;
        }

        virtual ~CashFlow() { ; }

        virtual StartCashFlow to_start_cashflow() const = 0;
        virtual EndCashFlow to_end_cashflow() const = 0;
        virtual CouponCashFlow to_coupon(const InterestRate& growth = InterestRate::ZERO) const = 0;
        virtual CustomCashFlow to_custom_cashflow(const Maturity& maturity) const = 0;
        virtual CustomCashFlow to_custom_cashflow(const Maturity& maturity, const InterestRate& other_interest_rate) const = 0;

    protected:
        TermStructure term;
    public:
        double cash;
    };

    class CouponCashFlow : public CashFlow
    {
    public:
        explicit CouponCashFlow(const Schedule& cal_, const InterestRate& ir_, double cash_, const InterestRate& growth_ = InterestRate::ZERO)
                : CashFlow(cal_, ir_, cash_)
                , growth(growth_)
        {

        }

        explicit CouponCashFlow(const TermStructure& term_, double cash_, const InterestRate& growth_ = InterestRate::ZERO)
                : CashFlow(term_, cash_)
                , growth(growth_)
        {

        }

        virtual StartCashFlow to_start_cashflow() const override;
        virtual EndCashFlow to_end_cashflow() const override;
        virtual CouponCashFlow to_coupon(const InterestRate& growth = InterestRate::ZERO) const override {return CouponCashFlow{term, cash};};
        virtual CustomCashFlow to_custom_cashflow(const Maturity& maturity) const override;
        virtual CustomCashFlow to_custom_cashflow(const Maturity& maturity, const InterestRate& other_interest_rate) const;

    public:
        InterestRate growth;
    };

    class StartCashFlow : public CashFlow
    {
    public:
        explicit StartCashFlow(const Schedule& cal_, const InterestRate& ir_, double cash_)
                : CashFlow(cal_, ir_, cash_)
        {
            ;
        }

        explicit StartCashFlow(const TermStructure& term_, double cash_)
                : CashFlow(term_, cash_)
        {
            ;
        }

        virtual StartCashFlow to_start_cashflow() const override {return *this;};
        virtual EndCashFlow to_end_cashflow() const override;
        virtual CouponCashFlow to_coupon(const InterestRate& growth = InterestRate::ZERO) const override
        {
            return coupon_from_npv(cash, term);
        }
        virtual CustomCashFlow to_custom_cashflow(const Maturity& maturity) const override;
        virtual CustomCashFlow to_custom_cashflow(const Maturity& maturity, const InterestRate& other_interest_rate) const;
    };

    class EndCashFlow : public CashFlow
    {
    public:
        explicit EndCashFlow(const Schedule& cal_, const InterestRate& ir_, double cash_)
                : CashFlow(cal_, ir_, cash_)
        {
            ;
        }

        explicit EndCashFlow(const TermStructure& term_, double cash_)
                : CashFlow(term_, cash_)
        {
            ;
        }

        virtual StartCashFlow to_start_cashflow() const override
        {
            return to_present_value(*this, term);
        }
        virtual EndCashFlow to_end_cashflow() const override
        {
            return *this;
        };
        virtual CouponCashFlow to_coupon(const InterestRate& growth = InterestRate::ZERO) const override
        {
            return coupon_from_fv(cash, term);
        }
        virtual CustomCashFlow to_custom_cashflow(const Maturity& maturity) const override;
        virtual CustomCashFlow to_custom_cashflow(const Maturity& maturity, const InterestRate& other_interest_rate) const override;
    };

    class CustomCashFlow : public CashFlow
    {
    public:
        explicit CustomCashFlow(const Schedule& cal_, const InterestRate& ir_, double cash_, const Maturity& maturity_)
                : CashFlow(cal_, ir_, cash_)
                , maturity(maturity_)
        {
            ;
        }

        explicit CustomCashFlow(const TermStructure& term_, double cash_, const Maturity& maturity_)
                : CashFlow(term_, cash_)
                , maturity(maturity_)
        {
            ;
        }

        virtual StartCashFlow to_start_cashflow() const override
        {
            InterestRate r = term.get_interpolated_interest_rate(maturity);
            double newcash = maturity_to_zero(cash, r, maturity);
            return StartCashFlow{term, newcash};
        }
        virtual EndCashFlow to_end_cashflow() const override
        {
            return to_future_value(to_start_cashflow(), term);
        }
        virtual CouponCashFlow to_coupon(const InterestRate& growth = InterestRate::ZERO) const override
        {
            // TODO: is ok ? growth ?
            return CouponCashFlow(term, cash);  // , growth
        };
        virtual CustomCashFlow to_custom_cashflow(const Maturity& maturity) const override;
        virtual CustomCashFlow to_custom_cashflow(const Maturity& maturity, const InterestRate& other_interest_rate) const override;

        const Maturity& get_maturity() const
        {
            return maturity;
        }
    protected:
        Maturity maturity;
    };

}

#endif //RED_PANDAS_PROJECT_NPV_H
