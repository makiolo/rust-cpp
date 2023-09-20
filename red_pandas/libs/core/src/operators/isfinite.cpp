#include "operators/less.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"

namespace rp {

    class IsFinite : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Less);

        template <typename T>
        explicit IsFinite(T&& s0, bool any_isfinite = true) {

            const auto& n0 = get_value< Serie::Buffer >(s0);

            if (any_isfinite)
                result = nc::any(!nc::isnan(n0) && !nc::isinf(n0))[0];
            else
                result = nc::all(!nc::isnan(n0) && !nc::isinf(n0))[0];
        }

        bool result;
    };

    bool any_isfinite(const std::shared_ptr<Serie> &s0) {
        return IsFinite(s0, true).result;
    }

    bool all_isfinite(const std::shared_ptr<Serie> &s0) {
        return IsFinite(s0, false).result;
    }

    bool any_isfinite(const Serie& s0)
    {
        return IsFinite(s0, true).result;
    }

    bool all_isfinite(const Serie& s0)
    {
        return IsFinite(s0, false).result;
    }

}
