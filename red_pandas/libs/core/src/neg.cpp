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
            auto ticket = make_ticket();
            _task = std::jthread([](const ticket_type& ticket, T &&ss0) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif

                auto s0 = rp::calculate(ss0);
                const auto& n0 = get_value< Serie::Buffer >(s0);
                ticket->set_value(std::make_shared<Serie>(-n0));

            }, ticket, std::forward<T>(s0));
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
