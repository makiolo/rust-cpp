#include "rsi.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"
#include <ta_libc.h>
#include "sum.h"

namespace rp {

    class Rsi : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Rsi);

        template <typename T>
        explicit Rsi(T&& s0, int period) {
            _future = _promise.get_future();
            _task = std::thread([](std::promise<result_type>& promise, const T& ss0, int period) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif

                auto s0 = rp::calculate(ss0);
                const auto& n0 = get_value< Serie::Buffer >(s0);

                int len = n0.numCols();
                if(len < period)
                {
                    promise.set_value(std::make_shared<Serie>(Serie::Buffer({NAN})));
                }
                else
                {
                    auto *output = new double[len];

                    int startIdx = 0;
                    int endIdx = len - 1;
                    int out_offset;
                    int out_total_elements;

                    std::fill_n(output, period, NAN);

                    TA_RetCode rs = TA_RSI(startIdx,
                                           endIdx,
                                           &n0[0],
                                           // begin parameters
                                           period,
                                           // end parameters
                                           &out_offset,
                                           &out_total_elements,
                                           &output[period]);
                    if (rs != TA_SUCCESS)
                        throw std::runtime_error("Error calculating indicator.");

                    promise.set_value(std::make_shared<Serie>(output, len, true));
                }

            }, std::ref(_promise), std::forward<T>(s0), period);
            _task.join();
        }
    };

    std::shared_ptr<Serie> rsi(const std::shared_ptr<Serie> &s0, int period)
    {
        return std::make_shared<Serie>(Rsi(s0, period));
    }

    Serie rsi(const Serie& s0, int period)
    {
        return Serie{Rsi{s0, period}};
    }
}
