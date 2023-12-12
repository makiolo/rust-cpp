#include "sqrt.h"
#include "util.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"

namespace rp {

    class RandNormal : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(RandNormal);

        explicit RandNormal(int n) {
            auto ticket = make_ticket();
            _task = std::jthread([](const ticket_type& ticket, int nn) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif
                ticket->set_value(std::make_shared<Serie>(nc::random::randN<double>(nc::Shape(1, nn))));

            }, ticket, n);
        }
    };

    std::shared_ptr<Serie> rand_normal(int n)
    {
        return std::make_shared<Serie>(RandNormal(n));
    }

    /*
    Serie rand_normal(int n)
    {
        return Serie{RandNormal{n}};
    }
    */
}
