#include "range.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"
#include <ta_libc.h>
#include "sum.h"

namespace rp {

    class Seq : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Seq);

        explicit Seq(int elements) {
            auto ticket = make_ticket();
            _task = std::jthread([](const ticket_type& ticket, int elements) -> void {

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

                ticket->set_value(std::make_shared<Serie>(output, num_elements, true));

            }, ticket, elements);
        }
    };

    std::shared_ptr<Serie> seq(int elements)
    {
        return std::make_shared<Serie>(Seq(elements));
    }

    Serie seqref(int elements)
    {
        return Serie{Seq{elements}};
    }
}
