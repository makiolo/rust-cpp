#include "sqrt.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"

namespace rp {

    class Sqrt : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Sqrt);

        template <typename T>
        explicit Sqrt(T&& s0) {
            _future = _promise.get_future();
            _task = std::jthread([](std::promise<result_type> &promise, const T& ss0) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif

                const auto& n0 = get_value< Serie::Buffer >(ss0);
                promise.set_value(std::make_shared<Serie>(nc::sqrt(n0)));

            }, std::ref(_promise), std::forward<T>(s0));
            // _task.join();
        }
    };

    std::shared_ptr<Serie> sqrt(const std::shared_ptr<Serie> &s0)
    {
        return std::make_shared<Serie>(Sqrt(s0));
    }

    Serie sqrtref(const Serie& s0)
    {
        return Serie{Sqrt{s0}};
    }
}
