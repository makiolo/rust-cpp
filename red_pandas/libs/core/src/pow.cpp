#include "pow.h"
//
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <execution>
#include <future>
#include <thread>
#include "util.h"

namespace rp {

    class Pow : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Pow);

        template <typename T>
        Pow(T&& s0, T&& s1) {
            _future = _promise.get_future();
            _task = std::thread([](std::promise<result_type> &promise, const T& ss0, const T& ss1) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif

                const auto& n0 = get_value< Serie::Buffer >(ss0);
                const auto& n1 = get_value< Serie::Buffer >(ss1);

                if (n0.numCols() == n1.numCols()) {
                    promise.set_value(std::make_shared<Serie>(nc::power(n0, n1.template astype<nc::uint8>())));
                } else if (n0.numCols() == 1) {
                    Serie::Buffer n0_expanded = n0.repeat(n1.numRows(), n1.numCols());
                    promise.set_value(std::make_shared<Serie>(nc::power(n0_expanded, n1.template astype<nc::uint8>())));
                } else if (n1.numCols() == 1) {
                    promise.set_value(std::make_shared<Serie>(nc::power(n0, n1.template astype<nc::uint8>()[0])));
                } else if (n0.numCols() > 1) {
                    throw std::runtime_error("TODO: Implement excel auto-fill");
                } else if (n1.numCols() > 1) // 2 <= x < N
                {
                    throw std::runtime_error("TODO: Implement excel auto-fill");
                } else {
                    throw std::runtime_error("Invalid shapes on Sum.");
                }

            }, std::ref(_promise), std::forward<T>(s0), std::forward<T>(s1));
            _task.join();
        }
    };

    std::shared_ptr<Serie> pow(const std::shared_ptr<Serie> &s0, const std::shared_ptr<Serie> &s1)
    {
        return std::make_shared<Serie>(Pow(s0, s1));
    }

    Serie powref(const Serie& s0, const Serie& s1)
    {
        return Serie{Pow{s0, s1}};
    }
}
