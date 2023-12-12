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
            auto ticket = make_ticket();
            _task = std::jthread([](const ticket_type& ticket, T &&ss0) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif
                auto s0 = rp::calculate(ss0);

                const auto& n0 = get_value< Serie::Buffer >(s0);
                ticket->set_value(std::make_shared<Serie>(nc::exp(n0)));

            }, ticket, std::forward<T>(s0));
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
