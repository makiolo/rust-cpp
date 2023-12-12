#include "shift.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"
#include <ta_libc.h>

namespace rp {

    class Shift : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Shift);

        template <typename T>
        explicit Shift(T&& s0, int period) {
            auto ticket = make_ticket();
            _task = std::jthread([](const ticket_type& ticket, T &&ss0, int period) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif
                auto s0 = rp::calculate(ss0);

                const auto& n0 = get_value< Serie::Buffer >(s0);

                const double* data = const_raw_data(s0);
                size_t num_elements = get_size(s0);

                column_ptr result;
                Shift::calculate(data, num_elements, period, result);
                ticket->set_value(result);

            }, ticket, std::forward<T>(s0), period);
        }

        static void calculate(const double* data, size_t num_elements, int period, column_ptr& result)
        {
            auto* output = new double[num_elements];

            if(period >= 0)
            {
                std::fill_n(output, period, NAN);
                for(size_t i = period; i < num_elements; ++i)
                    output[i] = data[i - period];
            }
            else // period negative
            {
                for(size_t i = 0; i < num_elements + period; ++i)
                    output[i] = data[i - period];
                for(size_t i = num_elements + period; i < num_elements; ++i)
                    output[i] = NAN;
            }
            result = std::make_shared<Serie>(output, num_elements, true);
        }
    };

    std::shared_ptr<Serie> shift(const std::shared_ptr<Serie> &s0, int period)
    {
        return std::make_shared<Serie>(Shift(s0, period));
    }

    Serie shift(const Serie& s0, int period)
    {
        return Serie{Shift{s0, period}};
    }
}
