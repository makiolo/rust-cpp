#include "neg.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"

namespace rp {

    class Neg : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Neg);

        template <typename T>
        explicit Neg(T&& s0) {
            _future = _promise.get_future();
            _task = std::jthread([=](std::promise<result_type> &promise, const T& ss0) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif

                const auto& n0 = get_value< Serie::Buffer >(ss0);
                promise.set_value(std::make_shared<Serie>(-n0));

            }, std::ref(_promise), std::forward<T>(s0));
        }
    };

    std::shared_ptr<Serie> neg(const std::shared_ptr<Serie> &s0)
    {
        return std::make_shared<Serie>(Neg(s0));
    }

    Serie neg(const Serie& s0)
    {
        return Serie{Neg{s0}};
    }
}
