#include "pdf.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"


namespace nc
{
    #define NC_TWO_MATH_PIl (2 * 3.1415926535897932384626433832795029L)

    template<typename dtype>
    auto pdf(dtype x) noexcept
    {
        STATIC_ASSERT_ARITHMETIC_OR_COMPLEX(dtype);

        // m=0
        // v=1
        // return rp.exp(rp.neg(rp.pow(x - rp.Serie(m), rp.static.TWO)) / (rp.static.TWO * (rp.pow(rp.Serie(v), rp.static.TWO)))) / (rp.Serie(v) * rp.sqrt(rp.static.TWO_PI))
        //
        return std::exp(-std::pow(x, 2.0) / 2.0) / std::sqrt(NC_TWO_MATH_PIl);
    }

    template<typename dtype>
    auto pdf(const NdArray<dtype>& inArray)
    {
        NdArray<decltype(pdf(dtype{0}))> returnArray(inArray.shape());
        stl_algorithms::transform(inArray.cbegin(), inArray.cend(), returnArray.begin(),
                                  [](dtype inValue) noexcept -> auto
                                  {
                                      return pdf(inValue);
                                  });

        return returnArray;
    }
}  // namespace nc

namespace rp {

    class Pdf : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Pdf);

        template <typename T>
        explicit Pdf(T&& s0) {
            auto ticket = make_ticket();
            _task = std::jthread([](const ticket_type& ticket, T &&ss0) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif
                auto s0 = rp::calculate(ss0);
                const auto& n0 = get_value< Serie::Buffer >(s0);
                ticket->set_value(std::make_shared<Serie>(nc::pdf(n0).template astype<double>()));

            }, ticket, std::forward<T>(s0));
        }
    };

    std::shared_ptr<Serie> pdf(const std::shared_ptr<Serie> &s0)
    {
        return std::make_shared<Serie>(Pdf(s0));
    }

    Serie pdf(const Serie& s0)
    {
        return Serie{Pdf{s0}};
    }
}
