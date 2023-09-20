#include "operators/less.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"

namespace rp {

    class Less : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Less);

        template <typename T>
        explicit Less(T&& s0, T&& s1, bool any_less = true) {

            const auto& n0 = get_value< Serie::Buffer >(s0);
            const auto& n1 = get_value< Serie::Buffer >(s1);

            if (n0.numCols() == n1.numCols()) {
                if (any_less)
                    result = nc::any(n0 < n1)[0];
                else
                    result = nc::all(n0 < n1)[0];
            } else if (n0.numCols() == 1) {
                if (any_less)
                    result = nc::any(n0[0] < n1)[0];
                else
                    result = nc::all(n0[0] < n1)[0];
            } else if (n1.numCols() == 1) {
                if (any_less)
                    result = nc::any(n0 < n1[0])[0];
                else
                    result = nc::all(n0 < n1[0])[0];
            } else if (n0.numCols() > 1) {
                throw std::runtime_error("TODO: Implement excel auto-fill");
            } else if (n1.numCols() > 1) {
                throw std::runtime_error("TODO: Implement excel auto-fill");
            } else {
                throw std::runtime_error("Invalid shapes on Sum.");
            }
        }

        bool result;
    };

    bool any_less(const std::shared_ptr<Serie> &s0, const std::shared_ptr<Serie> &s1) {
        return Less(s0, s1, true).result;
    }

    bool all_less(const std::shared_ptr<Serie> &s0, const std::shared_ptr<Serie> &s1) {
        return Less(s0, s1, false).result;
    }

    bool any_less(const Serie& s0, const Serie& s1)
    {
        return Less(s0, s1, true).result;
    }

    bool all_less(const Serie& s0, const Serie& s1)
    {
        return Less(s0, s1, false).result;
    }

    // sqrt ?
    // erf
    // normal_cdf
    // normal_pdf
    //
    /*

    def pdf(x, m=0, v=1):
        return math.exp(-((x - m) ** 2) / (2 * (v**2))) / (v * math.sqrt(2 * np.pi))

    def cdf(x, m=0, v=1):
        return (math.erf((math.sqrt(2) * (x - m)) / (2 * v)) / 2) + 0.5

    def d1(S, K, r, q, Vol, T):
        return (np.log(S / K) + (r - q + Vol ** 2 / 2) * T) / Vol * np.sqrt(T)

    def d2(d1, Vol, T):
        return d1 - Vol * np.sqrt(T)

    def BSMCall(S, K, r, q, Vol, T):
        d_uno = d1(S, K, r, q, Vol, T)
        d_dos = d2(d_uno, Vol, T)
        return (S * np.exp(-q * T) * cdf(d_uno)) - (K * np.exp(-r * T) * cdf(d_dos))

    def BSMPut(S, K, r, q, Vol, T):
        d_uno = d1(S, K, r, q, Vol, T)
        d_dos = d2(d_uno, Vol, T)
        return (K * np.exp(-r * T) * cdf(-d_dos)) - (S * np.exp(-q * T) * cdf(-d_uno))

    def BSMCallGreeks(S, K, r, q, Vol, T):
        d_uno = d1(S, K, r, q, Vol, T)
        d_dos = d2(d_uno, Vol, T)
        Delta = cdf(d_uno)
        Gamma = pdf(d_uno) / (S * Vol * np.sqrt(T))
        Theta = -(S * Vol * pdf(d_uno)) / (2 * np.sqrt(T) - r*K*np.exp(-r*T) * cdf(d_dos))
        Vega = S * np.sqrt(T) * pdf(d_uno)
        Rho = K * T * np.exp(-r*T) * cdf(d_dos)
        return Delta, Gamma, Theta, Vega , Rho

    def BSMPutGreeks(S, K, r, q, Vol, T):
        d_uno = d1(S, K, r, q, Vol, T)
        d_dos = d2(d_uno, Vol, T)
        Delta = cdf(-d_uno)
        Gamma = pdf(d_uno) / (S * Vol * np.sqrt(T))
        Theta = -(S*Vol*pdf(d_uno)) / (2*np.sqrt(T))+ r*K * np.exp(-r*T) * cdf(-d_uno)
        Vega = S * np.sqrt(T) * pdf(d_uno)
        Rho = -K*T*np.exp(-r*T) * cdf(-d_dos)
        return Delta, Gamma, Theta, Vega, Rho
    */
    // Implement own pdf - cdf
    // https://en.wikipedia.org/wiki/Gauss%E2%80%93Hermite_quadrature
    // 1. encontrar el pdf por algoritmo geneticos
    // 2. encontrar el cdf con sympy
    // 3. calcular griegas


    // 1. calcular modelo DL A. Input: datos de precios. Output: cdf y pdf
    // 2. generar griegas
    // 3. calcular modelo DL B. Input precios y griegas. Output: precio siguiente vela
    // 4. generar caminos utilizando B.
    // 5. calcular el max percent
    // 6. calcular el min percent
    // 7. calcular el reward ratio y evaluar oportunidad para entrar o no entrar.

}
