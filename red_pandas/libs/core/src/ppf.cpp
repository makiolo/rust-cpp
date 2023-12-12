#include "ppf.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"



namespace nc
{
    /*
 *
 * Inverse of Normal distribution function
 *
 *
 *
 * SYNOPSIS:
 *
 * double x, y, ndtri();
 *
 * x = ndtri( y );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the argument, x, for which the area under the
 * Gaussian probability density function (integrated from
 * minus infinity to x) is equal to y.
 *
 *
 * For small arguments 0 < y < exp(-2), the program computes
 * z = sqrt( -2.0 * log(y) );  then the approximation is
 * x = z - log(z)/z  - (1/z) P(1/z) / Q(1/z).
 * There are two rational functions P/Q, one for 0 < y < exp(-32)
 * and the other for y up to exp(-2).  For larger arguments,
 * w = y - 0.5, and  x/sqrt(2pi) = w + w**3 R(w**2)/S(w**2)).
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain        # trials      peak         rms
 *    DEC      0.125, 1         5500       9.5e-17     2.1e-17
 *    DEC      6e-39, 0.135     3500       5.7e-17     1.3e-17
 *    IEEE     0.125, 1        20000       7.2e-16     1.3e-16
 *    IEEE     3e-308, 0.135   50000       4.6e-16     9.8e-17
 *
 *
 * ERROR MESSAGES:
 *
 *   message         condition    value returned
 * ndtri domain       x <= 0        -MAXNUM
 * ndtri domain       x >= 1         MAXNUM
 *
 */

    /*
    Cephes Math Library Release 2.8:  June, 2000
    Copyright 1984, 1987, 1989, 2000 by Stephen L. Moshier
    */


    static double s2pi = 2.50662827463100050242E0;

    static double P0[5] = {
            -5.99633501014107895267E1,
            9.80010754185999661536E1,
            -5.66762857469070293439E1,
            1.39312609387279679503E1,
            -1.23916583867381258016E0,
    };
    static double Q0[8] = {
            1.95448858338141759834E0,
            4.67627912898881538453E0,
            8.63602421390890590575E1,
            -2.25462687854119370527E2,
            2.00260212380060660359E2,
            -8.20372256168333339912E1,
            1.59056225126211695515E1,
            -1.18331621121330003142E0,
    };
    static double P1[9] = {
            4.05544892305962419923E0,
            3.15251094599893866154E1,
            5.71628192246421288162E1,
            4.40805073893200834700E1,
            1.46849561928858024014E1,
            2.18663306850790267539E0,
            -1.40256079171354495875E-1,
            -3.50424626827848203418E-2,
            -8.57456785154685413611E-4,
    };
    static double Q1[8] = {
            1.57799883256466749731E1,
            4.53907635128879210584E1,
            4.13172038254672030440E1,
            1.50425385692907503408E1,
            2.50464946208309415979E0,
            -1.42182922854787788574E-1,
            -3.80806407691578277194E-2,
            -9.33259480895457427372E-4,
    };
    static double P2[9] = {
            3.23774891776946035970E0,
            6.91522889068984211695E0,
            3.93881025292474443415E0,
            1.33303460815807542389E0,
            2.01485389549179081538E-1,
            1.23716634817820021358E-2,
            3.01581553508235416007E-4,
            2.65806974686737550832E-6,
            6.23974539184983293730E-9,
    };
    static double Q2[8] = {
            6.02427039364742014255E0,
            3.67983563856160859403E0,
            1.37702099489081330271E0,
            2.16236993594496635890E-1,
            1.34204006088543189037E-2,
            3.28014464682127739104E-4,
            2.89247864745380683936E-6,
            6.79019408009981274425E-9,
    };

    /* Routines used within this implementation */


    /*
     * calculates a value of a polynomial of the form:
     * a[0]x^N+a[1]x^(N-1) + ... + a[N]
    */
    double Polynomialeval(double x, double* a, unsigned int N)
    {
        if (N==0) return a[0];
        else
        {
            double pom = a[0];
            for (unsigned int i=1; i <= N; i++)
                pom = pom *x + a[i];
            return pom;
        }
    }

    /*
 * calculates a value of a polynomial of the form:
 * x^N+a[0]x^(N-1) + ... + a[N-1]
*/
    double Polynomial1eval(double x, double* a, unsigned int N)
    {
        if (N==0) return a[0];
        else
        {
            double pom = x + a[0];
            for (unsigned int i=1; i < N; i++)
                pom = pom *x + a[i];
            return pom;
        }
    }

    double ndtri( double y0 )
    {
        double x, y, z, y2, x0, x1;
        int code;
        if( y0 <= 0.0 )
            return( - std::numeric_limits<double>::infinity() );
        if( y0 >= 1.0 )
            return( + std::numeric_limits<double>::infinity() );
        code = 1;
        y = y0;
        if( y > (1.0 - 0.13533528323661269189) )
        {
            y = 1.0 - y;
            code = 0;
        }
        if( y > 0.13533528323661269189 )
        {
            y = y - 0.5;
            y2 = y * y;
            x = y + y * (y2 * Polynomialeval( y2, P0, 4)/ Polynomial1eval( y2, Q0, 8 ));
            x = x * s2pi;
            return(x);
        }
        x = std::sqrt( -2.0 * std::log(y) );
        x0 = x - std::log(x)/x;
        z = 1.0/x;
        if( x < 8.0 )
            x1 = z * Polynomialeval( z, P1, 8 )/ Polynomial1eval ( z, Q1, 8 );
        else
            x1 = z * Polynomialeval( z, P2, 8 )/ Polynomial1eval( z, Q2, 8 );
        x = x0 - x1;
        if( code != 0 )
            x = -x;
        return( x );
    }

    template<typename dtype>
    auto ppf(dtype inValue) noexcept
    {
        STATIC_ASSERT_ARITHMETIC_OR_COMPLEX(dtype);

        return ndtri(inValue);
    }

    template<typename dtype>
    auto ppf(const NdArray<dtype>& inArray)
    {
        NdArray<decltype(ppf(dtype{0}))> returnArray(inArray.shape());
        stl_algorithms::transform(inArray.cbegin(), inArray.cend(), returnArray.begin(),
                                  [](dtype inValue) noexcept -> auto
                                  {
                                      return ppf(inValue);
                                  });

        return returnArray;
    }
}  // namespace nc


namespace rp {

    class Ppf : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Ppf);

        template <typename T>
        explicit Ppf(T&& s0) {
            auto ticket = make_ticket();
            _task = std::jthread([](const ticket_type& ticket, T &&ss0) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif

                auto s0 = rp::calculate(ss0);

                const auto& n0 = get_value< Serie::Buffer >(s0);

                ticket->set_value(std::make_shared<Serie>(nc::ppf(n0).template astype<double>()));

            }, ticket, std::forward<T>(s0));
        }
    };

    std::shared_ptr<Serie> ppf(const std::shared_ptr<Serie> &s0)
    {
        return std::make_shared<Serie>(Ppf(s0));
    }

    Serie ppf(const Serie& s0)
    {
        return Serie{Ppf{s0}};
    }

}
