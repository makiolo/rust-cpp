#include "log.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"

namespace rp {

    class Log : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Log);

        template <typename T>
        explicit Log(T&& s0) {
            _future = _promise.get_future();
            _task = std::jthread([](std::promise<result_type> &promise, const T& ss0) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif

                const auto& n0 = get_value< Serie::Buffer >(ss0);
                promise.set_value(std::make_shared<Serie>(nc::log(n0)));

            }, std::ref(_promise), std::forward<T>(s0));
            // _task.join();
        }
    };

    std::shared_ptr<Serie> log(const std::shared_ptr<Serie> &s0)
    {
        return std::make_shared<Serie>(Log(s0));
    }

    Serie log(const Serie& s0)
    {
        return Serie{Log{s0}};
    }
}
