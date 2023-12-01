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
            _future = _promise.get_future();
            _task = std::thread([](std::promise<result_type>& promise, const T& ss0, const T& ss1) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif

                const auto& n0 = get_value< Serie::Buffer >(ss0);
                const auto& n1 = get_value< Serie::Buffer >(ss1);

                if (n0.numCols() == n1.numCols()) {
                    promise.set_value(std::make_shared<Serie>(n0 / n1));
                } else if (n0.numCols() == 1) {
                    promise.set_value(std::make_shared<Serie>(n0[0] / n1));
                } else if (n1.numCols() == 1) {
                    promise.set_value(std::make_shared<Serie>(n0 / n1[0]));
                } else if (n0.numCols() > 1) {
                    throw std::runtime_error("TODO: Implement excel auto-fill");
                } else if (n1.numCols() > 1) // 2 <= x < N
                {
                    throw std::runtime_error("TODO: Implement excel auto-fill");
                } else {
                    throw std::runtime_error("Invalid shapes on Sub.");
                }

            }, std::ref(_promise), std::forward<T>(s0), std::forward<T>(s1));
            _task.join();
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
