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
            _future = _promise.get_future();
            _task = std::jthread([](std::promise<result_type> &promise, const T& ss0) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif

                const auto& n0 = get_value< Serie::Buffer >(ss0);
                promise.set_value(std::make_shared<Serie>(nc::erf(n0)));

            }, std::ref(_promise), std::forward<T>(s0));
            // _task.join();
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
