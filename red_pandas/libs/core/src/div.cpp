#include "div.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"

namespace rp {

    class Div : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Div);

        template<typename T>
        explicit Div(T&& s0, T&& s1) {
            auto ticket = make_ticket();
            _task = std::jthread([](const ticket_type& ticket, T &&ss0, T &&ss1) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif
                auto s0 = rp::calculate(ss0);
                auto s1 = rp::calculate(ss1);

                const auto& n0 = get_value< Serie::Buffer >(s0);
                const auto& n1 = get_value< Serie::Buffer >(s1);

                if (n0.numCols() == n1.numCols()) {
                    ticket->set_value(std::make_shared<Serie>(n0 / n1));
                } else if (n0.numCols() == 1) {
                    ticket->set_value(std::make_shared<Serie>(n0[0] / n1));
                } else if (n1.numCols() == 1) {
                    ticket->set_value(std::make_shared<Serie>(n0 / n1[0]));
                } else if (n0.numCols() > 1) {
                    throw std::runtime_error("TODO: Implement excel auto-fill");
                } else if (n1.numCols() > 1) // 2 <= x < N
                {
                    throw std::runtime_error("TODO: Implement excel auto-fill");
                } else {
                    throw std::runtime_error("Invalid shapes on Sub.");
                }

            }, ticket, std::forward<T>(s0), std::forward<T>(s1));
        }

    };

    std::shared_ptr<Serie> div2(const std::shared_ptr<Serie> &s0, const std::shared_ptr<Serie> &s1)
    {
        return std::make_shared<Serie>(Div(s0, s1));
    }

    Serie div2(const Serie& s0, const Serie& s1)
    {
        return Serie{Div{s0, s1}};
    }

    // Implementar operandos de 2
    // pow(a, b)

    // Implementar operandos de 1
    // exp(a)
    // log(b)

    // Implementar agregaciones:
    // sum(vec) -> scalar
    // prod(vec) -> scalar
    // last(vec) -> scalar
    // first(vec) -> scalar
    // max(vec) -> scalar
    // min(vec) -> scalar
    // mean(vec) -> scalar
    // median(vec) -> scalar
    // percentil(vec, 95) -> scalar

}
