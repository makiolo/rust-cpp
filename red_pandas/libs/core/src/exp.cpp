#include "exp.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"


namespace rp {

    class Exp : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Exp);

        template <typename T>
        explicit Exp(T&& s0) {
            _future = _promise.get_future();
            _task = std::jthread([](std::promise<result_type> &promise, const T& ss0) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif

                const auto& n0 = get_value< Serie::Buffer >(ss0);
                promise.set_value(std::make_shared<Serie>(nc::exp(n0)));

            }, std::ref(_promise), std::forward<T>(s0));
            // _task.join();
        }
    };

    std::shared_ptr<Serie> exp(const std::shared_ptr<Serie> &s0)
    {
        return std::make_shared<Serie>(Exp(s0));
    }

    Serie exp(const Serie& s0)
    {
        return Serie{Exp{s0}};
    }
}
