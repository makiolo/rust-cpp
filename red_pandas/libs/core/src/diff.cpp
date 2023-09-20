#include "diff.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"
#include <ta_libc.h>
#include "sum.h"

namespace rp {

    class Diff : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Diff);

        template <typename T>
        explicit Diff(T&& s0, int period) {
            _future = _promise.get_future();
            _task = std::jthread([](std::promise<result_type> &promise, const T& ss0, int period) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif

                auto s0 = rp::calculate(ss0);
                const auto& n0 = get_value< Serie::Buffer >(s0);
                const double* data = const_raw_data(s0);
                size_t num_elements = get_size(s0);

                // copy
                auto* output = new double[num_elements];

                if(period >=0)
                {
//                    for(size_t i = 0; i < period; ++i)
//                        output[i] = NAN;
                    std::fill_n(output, period, NAN);
                    for(size_t i = period; i < num_elements; ++i)
                        output[i] = data[i] - data[i - period];
                }
                else // period negative
                {
                    for(size_t i = 0; i < num_elements + period; ++i)
                        output[i] = data[i] - data[i - period];
                    for(size_t i = num_elements + period; i < num_elements; ++i)
                        output[i] = NAN;
                }

                promise.set_value(std::make_shared<Serie>(output, num_elements, true));

            }, std::ref(_promise), std::forward<T>(s0), period);
        }
    };

    std::shared_ptr<Serie> diff(const std::shared_ptr<Serie> &s0, int period)
    {
        return std::make_shared<Serie>(Diff(s0, period));
    }

    Serie diff(const Serie& s0, int period)
    {
        return Serie{Diff{s0, period}};
    }
}
