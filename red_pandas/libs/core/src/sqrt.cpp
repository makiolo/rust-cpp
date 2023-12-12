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
            auto ticket = make_ticket();
            _task = std::jthread([](const ticket_type& ticket, T &&ss0) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif
                auto s0 = rp::calculate(ss0);

                const auto& n0 = get_value< Serie::Buffer >(s0);
                ticket->set_value(std::make_shared<Serie>(nc::sqrt(n0)));

            }, ticket, std::forward<T>(s0));
            // _task.join();
        }

        static void calculate(const Serie::Buffer& n0, rp::column_ptr& result) {
            result = rp::array(nc::sqrt(n0));
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
