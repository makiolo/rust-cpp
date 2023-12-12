#include "neg.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"

namespace rp {

    class Clean : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Clean);

        template <typename T>
        explicit Clean(T&& s0, bool replace, double neutral) {
            auto ticket = make_ticket();
            _task = std::jthread([](const ticket_type& ticket, T &&ss0, bool replace, double neutral) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif
                auto s0 = rp::calculate(ss0);

                const auto& n0 = get_value< Serie::Buffer >(s0);

                std::vector<double> newarray;
                for(const auto& value : n0)
                {
                    if(std::isfinite(value)) {
                        newarray.emplace_back(value);
                    } else if(replace) {
                        newarray.emplace_back(neutral);
                    }
                }
                ticket->set_value(std::make_shared<Serie>(newarray));

            }, ticket, std::forward<T>(s0), replace, neutral);
        }
    };

    std::shared_ptr<Serie> clean(const std::shared_ptr<Serie> &s0, bool replace, double neutral)
    {
        return std::make_shared<Serie>(Clean(s0, replace, neutral));
    }

    Serie clean(const Serie& s0, bool replace, double neutral)
    {
        return Serie{Clean{s0, replace, neutral}};
    }
}
