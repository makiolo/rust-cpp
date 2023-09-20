#pragma once

#include <future>
#include <NumCpp.hpp>
#include <variant>
#include <future>
#include "memoize/memoize.hpp"


namespace rp {

template <typename T>
class Calculation {
public:
    using result_type = std::shared_ptr<T>;

    // using memoize1c = dp14::memoize<std::shared_ptr<Serie>, std::shared_ptr<Serie> >;
    // using memoize2c = dp14::memoize<std::shared_ptr<Serie>, std::shared_ptr<Serie>, std::shared_ptr<Serie> >;

    Calculation() = default;

    ~Calculation() = default;

    Calculation(const rp::Calculation<T> &other) = delete;

    Calculation<T>& operator=(const rp::Calculation<T> &other) = delete;

    Calculation(rp::Calculation<T>&& other) noexcept
            : _future(std::move(other._future))
    {

    }

    Calculation<T>& operator=(Calculation<T>&& other) noexcept {
        _future = std::move(other._future);
        return *this;
    }

    bool is_ready() const {
        return _future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }

    void wait() const {
        _future.wait();
    }

    void wait(long long duration) const {
        _future.wait_for(std::chrono::seconds(duration));
    }

    const result_type &get() const {
        return _future.get();
    }

protected:
    // copyable
    std::shared_future<result_type> _future;
    // non-copyable
    std::promise<result_type> _promise;
    std::jthread _task;
};

}
