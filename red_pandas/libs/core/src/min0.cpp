#include "min0.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"

namespace rp {

    class Min0 : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Min0);

        template <typename T>
        explicit Min0(T&& s0) {
            _future = _promise.get_future();
            _task = std::jthread([](std::promise<result_type> &promise, const T& ss0) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif

                const auto& n0 = get_value< Serie::Buffer >(ss0);
                promise.set_value(std::make_shared<Serie>(nc::minimum(n0, nc::zeros<double>(n0.numRows(), n0.numCols()))));

            }, std::ref(_promise), std::forward<T>(s0));
        }
    };

    class Min2 : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Min2);

        template <typename T>
        explicit Min2(T&& s0, T&& s1) {
            _future = _promise.get_future();
            _task = std::jthread([](std::promise<result_type> &promise, const T& ss0, const T& ss1) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif

                const auto& n0 = get_value< Serie::Buffer >(ss0);
                const auto& n1 = get_value< Serie::Buffer >(ss1);
                promise.set_value(std::make_shared<Serie>(nc::minimum(n0, n1)));

            }, std::ref(_promise), std::forward<T>(s0), std::forward<T>(s1));
        }
    };

    std::shared_ptr<Serie> min0(const std::shared_ptr<Serie> &s0)
    {
        return std::make_shared<Serie>(Min0(s0));
    }

    Serie min0ref(const Serie& s0)
    {
        return Serie{Min0{s0}};
    }

    std::shared_ptr<Serie> min2(const std::shared_ptr<Serie>& s0, const std::shared_ptr<Serie>& s1)
    {
        return std::make_shared<Serie>(Min2(s0, s1));
    }

    Serie min2ref(const Serie& s0, const Serie& s1)
    {
        return Serie{Min2(s0, s1)};
    }
}
