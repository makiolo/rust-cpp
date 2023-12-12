#include "max0.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"

namespace rp {

    class Max0 : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Max0);

        template <typename T>
        explicit Max0(T&& s0) {
            auto ticket = make_ticket();
            _task = std::jthread([](const ticket_type& ticket, T &&ss0) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif
                auto s0 = rp::calculate(ss0);

                const auto& n0 = get_value< Serie::Buffer >(s0);
                ticket->set_value(std::make_shared<Serie>(nc::maximum(n0, nc::zeros<double>(n0.numRows(), n0.numCols()))));

            }, ticket, std::forward<T>(s0));
        }
    };

    class Max2 : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Max2);

        template <typename T>
        explicit Max2(T&& s0, T&& s1) {
            auto ticket = make_ticket();
            _task = std::jthread([](const ticket_type& ticket, T &&ss0, T &&ss1) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif

                const auto& n0 = get_value< Serie::Buffer >(ss0);
                const auto& n1 = get_value< Serie::Buffer >(ss1);
                ticket->set_value(std::make_shared<Serie>(nc::maximum(n0, n1)));

            }, ticket, std::forward<T>(s0), std::forward<T>(s1));
        }
    };

    std::shared_ptr<Serie> max0(const std::shared_ptr<Serie> &s0)
    {
        return std::make_shared<Serie>(Max0(s0));
    }

    Serie max0ref(const Serie& s0)
    {
        return Serie{Max0{s0}};
    }

    std::shared_ptr<Serie> max2(const std::shared_ptr<Serie>& s0, const std::shared_ptr<Serie>& s1)
    {
        return std::make_shared<Serie>(Max2(s0, s1));
    }

    Serie max2ref(const Serie& s0, const Serie& s1)
    {
        return Serie{Max2(s0, s1)};
    }
}
