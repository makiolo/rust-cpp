#include "range.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"
#include <ta_libc.h>
#include "sum.h"

namespace rp {

    class Range : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Range);

        explicit Range(int elements) {
            _future = _promise.get_future();
            _task = std::thread([](std::promise<result_type>& promise, int elements) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif

                int num_elements = std::abs(elements);
                auto *output = new double[num_elements];

                if(elements >= 0)
                {
                    for(int i = 0; i < elements; ++i)
                    {
                        output[i] = double(i);
                    }
                }
                else
                {
                    for(int i = 0; i > elements; --i)
                    {
                        output[-i] = double(i);
                    }
                }

                promise.set_value(std::make_shared<Serie>(output, num_elements, true));

            }, std::ref(_promise), elements);
            _task.join();
        }
    };

    std::shared_ptr<Serie> range(int elements)
    {
        return std::make_shared<Serie>(Range(elements));
    }

    Serie rangeref(int elements)
    {
        return Serie{Range{elements}};
    }
}
