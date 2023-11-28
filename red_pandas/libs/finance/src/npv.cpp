#include "npv.h"

namespace qs {


    const Maturity Maturity::ZERO(0.0);
    const Maturity Maturity::ONE(1.0);

    const InterestRate InterestRate::ZERO(0.0);

    double Leg::npv() const
    {
        double npv = 0.0;
        for(const auto &v : flows)
        {
            npv += v->to_start_cashflow().cash;
        }
        return npv;
    }

    void Leg::add(const std::shared_ptr<const CustomCashFlow>& flow) {
        flows.emplace_back(flow);
    }

    void Leg::add(const CustomCashFlow& flow) {
        flows.emplace_back(std::make_shared<const CustomCashFlow>(flow));
    }

    CustomCashFlow TermStructure::make_cashflow(const Maturity& maturity, double cash)
    {
        return CustomCashFlow{*this, cash, maturity};
    }

    CustomCashFlow StartCashFlow::to_custom_cashflow(const Maturity& other_maturity) const
    {
        Maturity first_mat = Maturity::ZERO;
        InterestRate r = term.get_interpolated_interest_rate(first_mat);
        double newcash = maturity_to_maturity(cash, r, first_mat, other_maturity);
        return CustomCashFlow(term, newcash, other_maturity);
    };

    CustomCashFlow EndCashFlow::to_custom_cashflow(const Maturity& other_maturity) const
    {
        Maturity last_mat = term.get_last_maturity();
        InterestRate r = term.get_interpolated_interest_rate(last_mat);
        double newcash = maturity_to_maturity(cash, r, last_mat, other_maturity);
        return CustomCashFlow(term, newcash, other_maturity);
    };

    CustomCashFlow CouponCashFlow::to_custom_cashflow(const Maturity& other_maturity) const
    {
        return to_start_cashflow().to_custom_cashflow(other_maturity);
    }

    CustomCashFlow CustomCashFlow::to_custom_cashflow(const Maturity& other_maturity) const
    {
        InterestRate r = term.get_interpolated_interest_rate(maturity);
        return CustomCashFlow(term, maturity_to_maturity(cash, r, maturity, other_maturity), other_maturity);
    };

    CustomCashFlow StartCashFlow::to_custom_cashflow(const Maturity& other_maturity, const InterestRate& other_interest_rate) const
    {
        Maturity first_mat = Maturity::ZERO;
        InterestRate r = term.get_interpolated_interest_rate(first_mat);
        double newcash = maturity_to_maturity(cash, r, other_interest_rate, first_mat, other_maturity);
        return CustomCashFlow(term, newcash, other_maturity);
    };

    CustomCashFlow EndCashFlow::to_custom_cashflow(const Maturity& other_maturity, const InterestRate& other_interest_rate) const
    {
        Maturity last_mat = term.get_last_maturity();
        InterestRate r = term.get_interpolated_interest_rate(last_mat);
        double newcash = maturity_to_maturity(cash, r, other_interest_rate, last_mat, other_maturity);
        return CustomCashFlow(term, newcash, other_maturity);
    };

    CustomCashFlow CouponCashFlow::to_custom_cashflow(const Maturity& other_maturity, const InterestRate& other_interest_rate) const
    {
        return to_start_cashflow().to_custom_cashflow(other_maturity, other_interest_rate);
    }

    CustomCashFlow CustomCashFlow::to_custom_cashflow(const Maturity& other_maturity, const InterestRate& other_interest_rate) const
    {
        InterestRate r = term.get_interpolated_interest_rate(maturity);
        double newcash = maturity_to_maturity(cash, r, other_interest_rate, maturity, other_maturity);
        return CustomCashFlow(term, newcash, other_maturity);
    };

    InterestRate DiscountFactor::to_interest_rate(const Maturity& maturity, Convention convention_, int compound_times_) const
    {
        return InterestRate(discount2rate(value, maturity.value, convention_, compound_times_), convention_, compound_times_);
    }

    [[nodiscard]] StartCashFlow CouponCashFlow::to_start_cashflow() const
    {
        return npv_from_growth_coupon(cash, growth, term);
    }

    [[nodiscard]] EndCashFlow CouponCashFlow::to_end_cashflow() const
    {
        return fv_from_growth_coupon(cash, growth, term);
    }

    [[nodiscard]] EndCashFlow StartCashFlow::to_end_cashflow() const
    {
        return to_future_value(*this, term);
    }

    [[nodiscard]] DiscountFactor InterestRate::to_discount_factor(const Maturity& maturity) const
    {
        return {rate2discount(value, maturity.value, conv, c)};
    }

    [[nodiscard]] InterestRate InterestRate::to_other_interest_rate(Convention other_convention, int other_compound_times) const
    {
        return equivalent_rate(value, conv, c, other_convention, other_compound_times);
    }

    [[nodiscard]] InterestRate InterestRate::to_other_interest_rate(const Maturity& maturity, Convention other_convention, int other_compound_times) const
    {
        return equivalent_rate(maturity, value, conv, c, other_convention, other_compound_times);
    }

    // ********************** //

    double discount2rate(double df, double maturity, Convention conv, int compound_times)
    {
        switch (conv)
        {
            case Convention::LINEAR:
                return (1.0 / df - 1.0) * (1.0 / maturity);
            case Convention::YIELD:
                return (pow(1.0 / df, 1.0 / (maturity * compound_times)) - 1.0) * compound_times;
            case Convention::EXPONENTIAL:
                return -log(df) / maturity;
            default:
                throw std::runtime_error("Invalid convention");
        }
    }

    double rate2discount(double zc, double maturity, Convention conv, int compound_times)
    {
        switch (conv)
        {
            case Convention::LINEAR:
                return 1.0 / (1.0 + zc * maturity);
            case Convention::YIELD:
                return 1.0 / (pow((1.0 + zc / compound_times), maturity * compound_times));
            case Convention::EXPONENTIAL:
                return exp(-zc * maturity);
            default:
                throw std::runtime_error("Invalid convention");
        }
    }

    // discount2logdiscount
    // logdiscount2discount
    // rate2logdiscount
    // logdiscount2rate

#define LOW_RATE  (-0.999)
#define HIGH_RATE  0.999
#define MAX_ITERATION 1000
#define PRECISION_REQ 0.00000001

    // IRR = calcula el mismo spot rate para distintos cashflows
    // InterestRate compute_irr(const std::vector<double>& cf, Convention convention, int compound_times)

    // PAR YIELD = calcula el mismo cashflow para distintos spot rates
    // InterestRate compute_par_yield(const TermStructure& term)

    // LAST SPOT RATE = calculo el último spot rate, a partir de un par yield
    // InterestRate compute_last_spot_rate(const TermStructure& term, const InterestRate& par_yield);

    InterestRate compute_irr(const std::vector<double>& cf, Convention convention, int compound_times)
    {
        int i = 0, j = 0;
        double m = 0.0;
        double old = 0.00;
        double new_ = 0.00;
        double oldguessRate = LOW_RATE;
        double newguessRate = LOW_RATE;
        double guessRate = LOW_RATE;
        double lowGuessRate = LOW_RATE;
        double highGuessRate = HIGH_RATE;
        double npv = 0.0;
        double discount_factor = 0.0;
        for (i = 0; i < MAX_ITERATION; i++)
        {
            npv = 0.00;
            for (j = 0; j < cf.size(); j++)
            {
                discount_factor = rate2discount(guessRate, j, convention, compound_times);
                npv = npv + (cf[j] * discount_factor);
            }
            /* Stop checking once the required precision is achieved */
            if ((npv > 0) && (npv < PRECISION_REQ))
                break;
            if (old == 0)
                old = npv;
            else
                old = new_;
            new_ = npv;
            if (i > 0)
            {
                if (old < new_)
                {
                    if (old < 0 && new_ < 0)
                        highGuessRate = newguessRate;
                    else
                        lowGuessRate = newguessRate;
                }
                else
                {
                    if (old > 0 && new_ > 0)
                        lowGuessRate = newguessRate;
                    else
                        highGuessRate = newguessRate;
                }
            }
            oldguessRate = guessRate;
            guessRate = (lowGuessRate + highGuessRate) / 2;
            newguessRate = guessRate;
        }
        return InterestRate(guessRate, convention, compound_times);
    }

    InterestRate equivalent_rate(const Maturity& maturity, double rate, Convention convention, int compound_times, Convention other_convention, int other_compound_times)
    {
        return InterestRate(rate, convention, compound_times)
                .to_discount_factor(maturity)
                .to_interest_rate(maturity, other_convention, other_compound_times);
    }

    InterestRate equivalent_rate(double rate, Convention convention, int compound_times, Convention other_convention, int other_compound_times)
    {
        return equivalent_rate(Maturity::ONE, rate, convention, compound_times, other_convention, other_compound_times);
    }

    InterestRate equivalent_rate(double rate, int compound_times, int other_compound_times)
    {
        return equivalent_rate(rate, Convention::YIELD, compound_times, Convention::YIELD, other_compound_times);
    }

    double maturity_to_zero(double cash, const InterestRate& r, const Maturity& maturity)
    {
        return cash * r.to_discount_factor(maturity).value;
    }

    double zero_to_maturity(double cash, const InterestRate& r, const Maturity& maturity)
    {
        return cash / r.to_discount_factor(maturity).value;
    }

    double maturity_to_maturity(double cash, const InterestRate& r, const Maturity& maturityA, const Maturity& maturityB)
    {
        return zero_to_maturity(maturity_to_zero(cash, r, maturityA), r, maturityB);
    }

    double maturity_to_maturity(double cash, const InterestRate& rA, const InterestRate& rB, const Maturity& maturityA, const Maturity& maturityB)
    {
        return zero_to_maturity(maturity_to_zero(cash, rA, maturityA), rB, maturityB);
    }

    StartCashFlow to_present_value(const EndCashFlow& cashflow, const TermStructure& term)
    {
        Maturity last_mat = term.get_last_maturity();
        InterestRate r = term.get_interpolated_interest_rate(last_mat);
        return StartCashFlow{term, maturity_to_zero(cashflow.cash, r, last_mat)};
    }

    EndCashFlow to_future_value(const StartCashFlow& cashflow, const TermStructure& term)
    {
        Maturity last_mat = term.get_last_maturity();
        InterestRate r = term.get_interpolated_interest_rate(last_mat);
        return EndCashFlow{term, zero_to_maturity(cashflow.cash, r, last_mat)};
    }

    StartCashFlow to_present_value(double cashflow, const InterestRate& r, const Schedule& cal)
    {
        auto maturity = cal.get_last_period().end;  // obtener maturity del cash (teniendo cierto "cal")
        return StartCashFlow{ cal, r, maturity_to_zero(cashflow, r, maturity) };
    }

    EndCashFlow to_future_value(double cashflow, const InterestRate& r, const Schedule& cal)
    {
        auto maturity = cal.get_last_period().end;  // obtener maturity del cash (teniendo cierto "cal")
        return EndCashFlow{ cal, r, zero_to_maturity(cashflow, r, maturity) };
    }

    // only coupons
    StartCashFlow npv_from_coupon(double coupon, const InterestRate& interest_rate, const Schedule& cal)
    {
        return npv_from_growth_coupon(coupon, InterestRate::ZERO, interest_rate, cal);
    }

    StartCashFlow npv_from_growth_coupon(double coupon, const InterestRate& growth_rate, const InterestRate& interest_rate, const Schedule& cal)
    {
        double npv = 0.0;
        double i = 0.0;
        for (const auto& df : interest_rate.get_discount_factors_end(cal))
        {
            // TODO: revisar ese i ?, que maturity es esa?
            npv += (df.value * (coupon / interest_rate.c)) / growth_rate.to_discount_factor(i).value;
            i += 1.0;
        }
        return StartCashFlow{cal, interest_rate, npv};
    }

    StartCashFlow npv_from_growth_coupon(double coupon, const InterestRate& growth_rate, const TermStructure& term)
    {
        double npv = 0.0;
        double i = 0.0;
        for (const auto& df : term.get_discount_factors_end())
        {
            // TODO: revisar ese i ?, que maturity es esa?
            npv += (df.value * (coupon / term._compound_times)) / growth_rate.to_discount_factor(i).value;
            i += 1.0;
        }
        return StartCashFlow{term, npv};
    }

    EndCashFlow fv_from_growth_coupon(double coupon, const InterestRate& growth_rate, const InterestRate& interest_rate, const Schedule& cal)
    {
        StartCashFlow npv = npv_from_growth_coupon(coupon, growth_rate, interest_rate, cal);
        return to_future_value(npv.cash, interest_rate, cal);
    }

    EndCashFlow fv_from_growth_coupon(double coupon, const InterestRate& growth_rate, const TermStructure& term)
    {
        StartCashFlow npv = npv_from_growth_coupon(coupon, growth_rate, term);
        return to_future_value(npv, term);
    }

    EndCashFlow fv_from_coupon(double coupon, const InterestRate& interest_rate, const Schedule& cal)
    {
        return fv_from_growth_coupon(coupon, InterestRate::ZERO, interest_rate, cal);
    }

    CouponCashFlow coupon_from_npv(double npv, const InterestRate& interest_rate, const Schedule& cal)
    {
        double total_df = 0.0;
        for (const auto& df : interest_rate.get_discount_factors_end(cal))
        {
            total_df += df.value;
        }
        return CouponCashFlow{cal, interest_rate, (npv * interest_rate.c) / total_df};
    }

    CouponCashFlow coupon_from_npv(double npv, const TermStructure& term)
    {
        double total_df = 0.0;
        for (const auto& df : term.get_discount_factors_end())
        {
            total_df += df.value;
        }
        // FIXME
        return CouponCashFlow{term, (npv * term._compound_times) / total_df};
    }

    // VAN = coupons - initial investement
    double classic_npv(double investment, double coupon, const InterestRate& interest_rate, const Schedule& cal)
    {
        return CouponCashFlow(cal, interest_rate, coupon).to_start_cashflow().cash - investment;
    }

    // coupons + payment on yield-maturity
    double bond_npv(double face_value, double coupon, const InterestRate& interest_rate, const Schedule& cal)
    {
        double coupons = CouponCashFlow(cal, interest_rate, coupon).to_start_cashflow().cash;
        double yield_on_payment = EndCashFlow(cal, interest_rate, face_value).to_start_cashflow().cash;
        return coupons + yield_on_payment;
    }

    double bond_npv(double face_value, double coupon, const TermStructure& term)
    {
        double coupons = CouponCashFlow(term, coupon).to_start_cashflow().cash;
        double yield_on_payment = EndCashFlow(term, face_value).to_start_cashflow().cash;
        return coupons + yield_on_payment;
    }

    // stock investment = coupons + payment on yield-maturity - initial investement
    double stock_npv(double investment, double dividend, const InterestRate& interest_rate, const Schedule& cal)
    {
        double coupon = investment * (dividend - interest_rate.value);
        double dividends = CouponCashFlow(cal, interest_rate, coupon).to_start_cashflow().cash;
        double yield_on_payment = EndCashFlow(cal, interest_rate, investment).to_start_cashflow().cash;
        double npv = dividends + yield_on_payment - investment;
        return npv;
    }

    CouponCashFlow coupon_from_growth_coupon(double coupon, const InterestRate& growth_rate, const InterestRate& interest_rate, const Schedule& cal)
    {
        StartCashFlow npv = npv_from_growth_coupon(coupon, growth_rate, interest_rate, cal);
        return coupon_from_npv(npv.cash, interest_rate, cal);
    }

    CouponCashFlow coupon_from_growth_coupon(double coupon, const InterestRate& growth_rate, const TermStructure& term)
    {
        StartCashFlow npv = npv_from_growth_coupon(coupon, growth_rate, term);
        return coupon_from_npv(npv.cash, term);
    }

    CouponCashFlow coupon_from_fv(double fv, const InterestRate& interest_rate, const Schedule& cal)
    {
        double total_df = 0.0;
        for (const auto& df : interest_rate.get_discount_factor_start(cal))
        {
            total_df += 1.0 / df.value;
        }
        return CouponCashFlow{cal, interest_rate, (fv * interest_rate.c) / total_df};
    }

    CouponCashFlow coupon_from_fv(double fv, const TermStructure& term)
    {
        double total_df = 0.0;
        for (const auto& df : term.get_discount_factor_start())
        {
            total_df += 1.0 / df.value;
        }
        // TODO: FIXME
        return CouponCashFlow{term, (fv * term._compound_times) / total_df};
    }

    InterestRate on_capital(double initial_value, double final_value, const Maturity& maturity, Convention convention, int compound_times)
    {
        if (convention == Convention::YIELD && compound_times == Frequency::ANNUAL)
        {
            // cagr
            return InterestRate(pow(final_value / initial_value, 1.0 / maturity.value) - 1.0);
        }
        else
        {
            return InterestRate((final_value - initial_value) / initial_value, Convention::LINEAR)
                    .to_discount_factor(Maturity::ONE)
                    .to_interest_rate(maturity, convention, compound_times);
        }
    }

    std::ostream& operator<<(std::ostream& out, const Maturity& obj)
    {
        if (obj.has_pillar)
        {
            out << obj.pillar << " (" << obj.value << ")";
        }
        else
        {
            out << obj.value;
        }
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const ForwardPeriod& obj)
    {
        out << "From period:\t" << obj.start << "\tTo period:\t" << obj.end;
        return out;
    }

    DiscountFactor ForwardPeriod::discount_factor(const InterestRate& ir) const
    {
        double df0 = ir.to_discount_factor(start).value;
        double df1 = ir.to_discount_factor(end).value;

        return DiscountFactor{ df1 / df0 };
    }

    DiscountFactor ForwardPeriod::discount_factor(const InterestRate& ir_start, const InterestRate& ir_end) const
    {
        double df0 = ir_start.to_discount_factor(start).value;
        double df1 = ir_end.to_discount_factor(end).value;

        return DiscountFactor{ df1 / df0 };
    }

    InterestRate ForwardPeriod::forward_rate(const InterestRate& ir) const
    {
        double df0 = ir.to_discount_factor(start).value;
        double df1 = ir.to_discount_factor(end).value;

        double m = end.value - start.value;

        return InterestRate((df0 / df1 - 1.0) / m);
    }

    InterestRate ForwardPeriod::forward_rate(const InterestRate& ir_start, const InterestRate& ir_end) const
    {
        double df0 = ir_start.to_discount_factor(start).value;
        double df1 = ir_end.to_discount_factor(end).value;

        double m = end.value - start.value;

        return InterestRate((df0 / df1 - 1.0) / m);
    }

    DiscountFactor ForwardPeriod::next_discount_factor(const InterestRate& ir) const
    {
        double discount = ir.to_discount_factor(end).value;
        double m = end.value - start.value;
        return { discount / (1.0 + m * forward_rate(ir).value) };
    }

    [[nodiscard]] DiscountFactor Maturity::get_discount_factor(const InterestRate& ir) const
    {
        return ir.to_discount_factor(*this);
    }

    std::vector<InterestRate> Schedule::spot_to_forward(const std::vector<InterestRate>& spots, Convention conv, int compound_times) const
    {
        std::vector<InterestRate> fwds;

        int i = 0;
        for (auto& period : get_forward_periods())
        {
            if (i == 0)
            {
                fwds.push_back(spots[i]);
            }
            else
            {
                InterestRate fwd = period.discount_factor(spots[i - 1], spots[i]).to_interest_rate(period.duration(), conv, compound_times);
                fwds.push_back(fwd);
            }
            i += 1;
        }
        return fwds;
    }

    double inverse_lerp(double min_value, double max_value, double v)
    {
        return (v - min_value) / (max_value - min_value);
    }

    double lerp(double min_value, double max_value, double t)
    {
        return min_value + t * (max_value - min_value);
    }

    [[nodiscard]] ForwardPeriod Maturity::to(const Maturity& target) const
    {
        return { *this, target };
    }
}
