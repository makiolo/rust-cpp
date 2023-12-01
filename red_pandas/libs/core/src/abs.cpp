#include "abs.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"

namespace rp {

    class Abs : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Abs);

        template <typename T>
        explicit Abs(T&& s0) {
            _future = _promise.get_future();
            _task = std::thread([](std::promise<result_type> &promise, const T& ss0) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif
                const auto& n0 = get_value< Serie::Buffer >(ss0);
                promise.set_value(std::make_shared<Serie>(nc::abs(n0)));

            }, std::ref(_promise), std::forward<T>(s0));
            _task.join();
        }
    };

    std::shared_ptr<Serie> abs(const std::shared_ptr<Serie> &s0)
    {
        return std::make_shared<Serie>(Abs(s0));
    }

    Serie abs(const Serie& s0)
    {
        return Serie{Abs{s0}};
    }
}
