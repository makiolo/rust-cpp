//
// Created by n424613 on 17/11/2022.
//

#ifndef RED_PANDAS_PROJECT_GREEKS_H
#define RED_PANDAS_PROJECT_GREEKS_H


#include "serie.h"
#include "sum.h"

namespace rp {

    rp::column_ptr year();
    rp::column_ptr zero();
    rp::column_ptr one();
    rp::column_ptr two();
    rp::column_ptr log2();
    rp::column_ptr four();
    rp::column_ptr onehundred();
    rp::column_ptr half();
    rp::column_ptr pi();
    rp::column_ptr two_pi();
    rp::column_ptr constant(double number);
    rp::column_ptr threshold();

    // Asset

    rp::column_ptr
    d1(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r, const rp::column_ptr &q,
       const rp::column_ptr &T, const rp::column_ptr &Vol);

    rp::column_ptr
    d2(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r, const rp::column_ptr &q,
       const rp::column_ptr &T, const rp::column_ptr &Vol);

    rp::column_ptr
    CallPrice(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r, const rp::column_ptr &q,
              const rp::column_ptr &T, const rp::column_ptr &Vol,
              const rp::column_ptr &d_uno = nullptr,
              const rp::column_ptr &d_dos = nullptr);

    rp::column_ptr
    PutPrice(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r, const rp::column_ptr &q,
             const rp::column_ptr &T, const rp::column_ptr &Vol,
             const rp::column_ptr &d_uno = nullptr,
             const rp::column_ptr &d_dos = nullptr);

    // Call Greeks

    rp::column_ptr
    CallGreeksDelta(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                    const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos);

    rp::column_ptr
    CallGreeksGamma(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                    const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos);

    rp::column_ptr
    CallGreeksTheta(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                    const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos);

    rp::column_ptr
    CallGreeksVega(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                   const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos);

    rp::column_ptr
    CallGreeksRho(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                  const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos);

    // Put Greeks

    rp::column_ptr
    PutGreeksDelta(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                   const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos);

    rp::column_ptr
    PutGreeksGamma(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                   const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos);

    rp::column_ptr
    PutGreeksTheta(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                   const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos);

    rp::column_ptr
    PutGreeksVega(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                  const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos);

    rp::column_ptr
    PutGreeksRho(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, const rp::column_ptr &T,
                 const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos);

    rp::column_ptr
    ImpliedVolatility(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r, const rp::column_ptr &q,
                      const rp::column_ptr &T, const rp::column_ptr &Vol_guess, const rp::column_ptr &market_price,
                      const std::string& flag, double tol = 0.0001, double lr = 0.33, long max_iter = 100);

    rp::column_ptr
    CallImpliedvolatility(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r, const rp::column_ptr &q,
                      const rp::column_ptr &T, const rp::column_ptr &Vol_guess, const rp::column_ptr &market_price);

    rp::column_ptr
    PutImpliedvolatility(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r, const rp::column_ptr &q,
                      const rp::column_ptr &T, const rp::column_ptr &Vol_guess, const rp::column_ptr &market_price);

    rp::column_ptr
    GreeksVolga(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r,  const rp::column_ptr &q, 
                const rp::column_ptr &T,
                const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos);

    rp::column_ptr
    Vanna(const rp::column_ptr &S, const rp::column_ptr &K, const rp::column_ptr &r, const rp::column_ptr &q,
                const rp::column_ptr &T,
                const rp::column_ptr &Vol, const rp::column_ptr &d_uno, const rp::column_ptr &d_dos);
}

#endif //RED_PANDAS_PROJECT_GREEKS_H

