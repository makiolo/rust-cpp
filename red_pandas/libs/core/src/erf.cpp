#include "erf.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"


namespace nc
{
    template<typename dtype>
    auto erf(dtype inValue) noexcept
    {
        STATIC_ASSERT_ARITHMETIC_OR_COMPLEX(dtype);

        return std::erf(inValue);
    }

    template<typename dtype>
    auto erf(const NdArray<dtype>& inArray)
    {
        NdArray<decltype(erf(dtype{0}))> returnArray(inArray.shape());
        stl_algorithms::transform(inArray.cbegin(), inArray.cend(), returnArray.begin(),
                                  [](dtype inValue) noexcept -> auto
                                  {
                                      return erf(inValue);
                                  });

        return returnArray;
    }
}  // namespace nc


namespace rp {

    class Erf : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Erf);

        template <typename T>
        explicit Erf(T&& s0) {
            auto ticket = make_ticket();
            _task = std::jthread([](const ticket_type& ticket, T &&ss0) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif
                auto s0 = rp::calculate(ss0);

                const auto& n0 = get_value< Serie::Buffer >(s0);
                ticket->set_value(std::make_shared<Serie>(nc::erf(n0)));

            }, ticket, std::forward<T>(s0));
        }
    };

    std::shared_ptr<Serie> erf(const std::shared_ptr<Serie> &s0)
    {
        return std::make_shared<Serie>(Erf(s0));
    }

    Serie erf(const Serie& s0)
    {
        return Serie{Erf{s0}};
    }
}
