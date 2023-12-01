#include "cdf.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include "util.h"


namespace nc
{
    #define NC_TWO_MATH_PIl (2 * 3.1415926535897932384626433832795029L)

    template<typename dtype>
    auto cdf(dtype x) noexcept
    {
        STATIC_ASSERT_ARITHMETIC_OR_COMPLEX(dtype);

        // m=0
        // v=1
        // return (rp.erf((rp.sqrt(rp.static.TWO) * x - rp.Serie(m)) / (rp.static.TWO * rp.Serie(v))) / rp.static.TWO) + rp.static.HALF
        //
        return (1.0 + std::erf(x / std::sqrt(2))) * 0.5;
    }

    template<typename dtype>
    auto cdf(const NdArray<dtype>& inArray)
    {
        NdArray<decltype(cdf(dtype{0}))> returnArray(inArray.shape());
        stl_algorithms::transform(inArray.cbegin(), inArray.cend(), returnArray.begin(),
                                  [](dtype inValue) noexcept -> auto
                                  {
                                      return cdf(inValue);
                                  });

        return returnArray;
    }
}  // namespace nc

namespace rp {

    class Cdf : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Cdf);

        template <typename T>
        explicit Cdf(T&& s0) {
            _future = _promise.get_future();
            _task = std::thread([](std::promise<result_type> &promise, const T& ss0) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif

                const auto& n0 = get_value< Serie::Buffer >(ss0);
                promise.set_value(std::make_shared<Serie>(nc::cdf(n0).template astype<double>()));

            }, std::ref(_promise), std::forward<T>(s0));
            _task.join();
        }
    };

    std::shared_ptr<Serie> cdf(const std::shared_ptr<Serie> &s0) {
        return std::make_shared<Serie>(Cdf(s0));
    }

    Serie cdf(const Serie& s0)
    {
        return Serie{Cdf{s0}};
    }
}
