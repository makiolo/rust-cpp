#include "formulas/greeks.h"
//
#include "div.h"
#include "exp.h"
#include "log.h"
#include "pow.h"
#include "cdf.h"
#include "pdf.h"
#include "sqrt.h"
#include "abs.h"
#include "operators/less.h"

namespace rp {

    static auto _zero = rp::array({0.0});
    static auto _one = rp::array({1.0});
    static auto _two = rp::array({2.0});
    static auto _log2 = rp::array({ 0.69314718055994530941723212145817656807550013436025L });
    static auto _four = rp::array({4.0});
    static auto _onehundred = rp::array({100.0});
    static auto _year = rp::array({365.0});
    static auto _half = rp::array({0.5});
    static auto _pi = rp::array({3.141592653589793238462643383279502884L});
    static auto _two_pi = rp::array({2 * 3.141592653589793238462643383279502884L});
    static auto _threshold = rp::array({1e-3});
    static thread_local auto cache_constants = std::unordered_map<double, rp::column_ptr>();

    rp::column_ptr year()
    {
        return _year;
    }

    rp::column_ptr zero()
    {
        return _zero;
    }

    rp::column_ptr one()
    {
        return _one;
    }

    rp::column_ptr two()
    {
        return _two;
    }

    rp::column_ptr log2()
    {
        return _log2;
    }

    rp::column_ptr four()
    {
        return _four;
    }

    rp::column_ptr onehundred()
    {
        return _onehundred;
    }

    rp::column_ptr half()
    {
        return _half;
    }

    rp::column_ptr pi()
    {
        return _pi;
    }

    rp::column_ptr two_pi()
    {
        return _two_pi;
    }

    rp::column_ptr threshold()
    {
        return _threshold;
    }

    rp::column_ptr constant(double number)
    {
        rp::column_ptr data;
        if (auto search = cache_constants.find(number); search != cache_constants.end()) {
            // found, reuse cache
            data = search->second;
        }
        else {
            // create and save in cache
            data = rp::array({number});
            cache_constants[number] = data;
        }
        return data;
    }

    rp::column_ptr
    d1(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r, const rp::column_ptr &q,
       const rp::column_ptr &T, const rp::column_ptr &Vol) {
        return (rp::log(S / K) + ((r - q + (rp::pow(Vol, two()) / two())) * T)) / (Vol * rp::sqrt(T));
    }

    rp::column_ptr
    d2(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r, const rp::column_ptr &q,
       const rp::column_ptr &T, const rp::column_ptr &Vol) {
        return d1(S, K, r, q, T, Vol) - (Vol * rp::sqrt(T));
    }

    rp::column_ptr
    CallPrice(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r, const rp::column_ptr &q,
              const rp::column_ptr &T, const rp::column_ptr &Vol,
              const rp::column_ptr &d_uno,
              const rp::column_ptr &d_dos) {
        rp::column_ptr duno = d_uno;
        rp::column_ptr ddos = d_dos;
        if (!d_uno)
            duno = d1(S, K, r, q, T, Vol);
        if (!d_dos)
            ddos = d2(S, K, r, q, T, Vol);
        return (S * rp::exp(-(q * T)) * rp::cdf(duno)) - (K * rp::exp(-(r * T)) * rp::cdf(ddos));
    }

    rp::column_ptr
    PutPrice(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r, const rp::column_ptr &q,
             const rp::column_ptr &T, const rp::column_ptr &Vol,
             const rp::column_ptr &d_uno,
             const rp::column_ptr &d_dos) {
        rp::column_ptr duno = d_uno;
        rp::column_ptr ddos = d_dos;
        if (!d_uno)
            duno = d1(S, K, r, q, T, Vol);
        if (!d_dos)
            ddos = d2(S, K, r, q, T, Vol);
        return (K * rp::exp(-(r * T)) * rp::cdf(-ddos)) - (S * rp::exp(-(q * T)) * rp::cdf(-duno));
    }

// Call Greeks

    rp::column_ptr
    CallGreeksDelta(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                    const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos) {
        return rp::exp(-q * T) * (rp::cdf(d_uno)) * onehundred();
    }

    rp::column_ptr
    CallGreeksGamma(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                    const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos) {
        return ((rp::exp(-q * T) * rp::pdf(d_uno)) / (S * Vol * rp::sqrt(T))) * onehundred();
    }

    rp::column_ptr
    CallGreeksTheta(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                    const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos) {
        // ANALITHIC 1
        // https://www.codearmo.com/python-tutorial/options-trading-greeks-black-scholes
        return (((-S * rp::exp(-q * T) * Vol * rp::pdf(d_uno)) / (two() * rp::sqrt(T))) - (r * K * rp::exp(-(r * T)) * rp::cdf(d_dos)) - (-q * S * rp::exp(-q * T) * rp::cdf(d_uno))) / year();

        //auto dt = rp::array({0.000001});
        //auto zero = rp::array({0.0});

        // CENTRAL
        //return -(CallPrice(S, K, r, zero, T + dt, Vol) - CallPrice(S, K, r, zero, T - dt, Vol)) / (two * dt);

        // FORWARD
        //return -(CallPrice(S, K, r, zero, T + dt, Vol) - CallPrice(S, K, r, zero, T, Vol)) / dt;

        // BACKWARD
        //return -(CallPrice(S, K, r, zero, T, Vol) - CallPrice(S, K, r, zero, T - dt, Vol)) / dt;
    }

    rp::column_ptr
    CallGreeksVega(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                   const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos) {
        return (S * rp::exp(-q * T) * rp::sqrt(T) * rp::pdf(d_uno)) / onehundred();
    }

    rp::column_ptr
    CallGreeksRho(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                  const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos) {
        return (K * T * rp::exp(-(r * T)) * rp::cdf(d_dos)) / onehundred();
    }

// Put Greeks

    rp::column_ptr
    PutGreeksDelta(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                   const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos) {
        return rp::exp(-q * T) * (rp::cdf(d_uno) - one()) * onehundred();
    }

    rp::column_ptr
    PutGreeksGamma(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                   const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos) {
        // same to call
        return ((rp::exp(-q * T) * rp::pdf(d_uno)) / (S * Vol * rp::sqrt(T))) * onehundred();
    }

    rp::column_ptr
    PutGreeksTheta(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                   const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos) {
        return (((-S * rp::exp(-q * T) * Vol * rp::pdf(d_uno)) / (two() * rp::sqrt(T))) + (r * K * rp::exp(-(r * T)) * rp::cdf(-d_dos)) - (q * S * rp::exp(-q * T) * rp::cdf(-d_uno))) / year();
    }

    rp::column_ptr
    PutGreeksVega(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                  const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos) {
        return (S * rp::exp(-q * T) * rp::sqrt(T) * rp::pdf(d_uno)) / onehundred();
    }

    rp::column_ptr
    PutGreeksRho(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                 const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos) {
        return (-K * T * rp::exp(-(r * T)) * rp::cdf(-d_dos)) / onehundred();
    }

    rp::column_ptr
    ImpliedVolatility(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r, const rp::column_ptr &q,
                      const rp::column_ptr &T, const rp::column_ptr &Vol_guess, const rp::column_ptr &market_price,
                      const std::string& flag, double tol, double lr, long max_iter)
    {
        auto tol_serie = rp::constant(tol);
        auto lr_serie = rp::constant(lr);
        rp::column_ptr sigma = Vol_guess;
        for(long i = 0; i < max_iter; ++i)
        {
            rp::column_ptr d_uno = d1(S, K, r, q, T, sigma);
            rp::column_ptr price;
            if (flag == "c")
                price = CallPrice(S, K, r, q, T, sigma, d_uno=d_uno);
            else
                price = PutPrice(S, K, r, q, T, sigma, d_uno=d_uno);
            rp::column_ptr diff = market_price - price;
            if (rp::all_less(rp::abs(diff), tol_serie))
                return sigma;
            rp::column_ptr Vega = S * rp::exp(-q * T) * rp::sqrt(T) * rp::pdf(d_uno);
            sigma = sigma + (diff / Vega) * lr_serie;
        }
        return sigma;
    }

    rp::column_ptr
    CallImpliedvolatility(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r, const rp::column_ptr &q,
                          const rp::column_ptr &T, const rp::column_ptr &Vol_guess, const rp::column_ptr &market_price)
    {
        return rp::ImpliedVolatility(S, K, r, q, T, Vol_guess, market_price, "c");
    }

    rp::column_ptr
    PutImpliedvolatility(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r, const rp::column_ptr &q,
                         const rp::column_ptr &T, const rp::column_ptr &Vol_guess, const rp::column_ptr &market_price)
    {
        return rp::ImpliedVolatility(S, K, r, q, T, Vol_guess, market_price, "p");
    }

    // volga or vomma
    rp::column_ptr
    GreeksVolga(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r, const rp::column_ptr &q, 
                const rp::column_ptr &T,
                const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos) {
        return rp::CallGreeksVega(S, K, r, q, T, Vol, d_uno, d_dos) * ((d_uno * d_dos) / Vol);
    }

}
