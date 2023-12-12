#pragma once

#include <future>
#include <NumCpp.hpp>
#include <variant>
#include <future>
#include <list>
#include "memoize/memoize.hpp"


namespace rp {

// rename to ticket
template <typename T>
struct ticket {
    using result_type = std::shared_ptr<T>;

    // copyable
    std::shared_future<result_type> _future;
    // non-copyable
    std::promise<result_type> _promise;

    explicit ticket()
    {
        _future = _promise.get_future();
    }
    ~ticket() = default;

    ticket(const rp::ticket<T> &other) = delete;
    ticket<T>& operator=(const rp::ticket<T> &other) = delete;

    ticket(rp::ticket<T>&& other) noexcept
        : _future(std::move(other._future))
    {
        ;
    }

    ticket<T>& operator=(ticket<T>&& other) noexcept {
        _future = std::move(other._future);
        return *this;
    }


    std::shared_future<result_type>& get_future()
    {
        return _future;
    }

    const std::shared_future<result_type>& get_future() const
    {
        return _future;
    }

    void set_value(const result_type& value)
    {
        _promise.set_value(value);
    }
};

template <typename T>
class Calculation : public std::enable_shared_from_this<Calculation<T> > {
public:
    using result_type = std::shared_ptr<T>;
    using ticket_type = std::shared_ptr<ticket<T> >;

    // using memoize1c = dp14::memoize<std::shared_ptr<Serie>, std::shared_ptr<Serie> >;
    // using memoize2c = dp14::memoize<std::shared_ptr<Serie>, std::shared_ptr<Serie>, std::shared_ptr<Serie> >;

    Calculation() = default;
    ~Calculation() = default;

    Calculation(const rp::Calculation<T> &other) = delete;
    Calculation<T>& operator=(const rp::Calculation<T> &other) = delete;

    Calculation(rp::Calculation<T>&& other) noexcept
            : _results(std::move(other._results))
//            , _future(std::move(other._future))
    {
        ;
    }

    Calculation<T>& operator=(Calculation<T>&& other) noexcept {
        if (this != &other) {
//            _future = std::move(other._future);
            _results = std::move(other._results);
        }
        return *this;
    }

    bool is_ready() const {
//        if(_results.size() > 0)
//        {
            return _results.front()->get_future().wait_for(std::chrono::seconds(0)) == std::future_status::ready;
//        }
//        else
//        {
//            return _future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
//        }
    }

    void wait() const {
//        if(_results.size() > 0)
            _results.front()->get_future().wait();
//        else
//            _future.wait();
    }

    void wait(long long duration) const {
//        if(_results.size() > 0)
            _results.front()->get_future().wait_for(std::chrono::seconds(duration));
//        else
//            _future.wait_for(std::chrono::seconds(duration));
    }

    result_type get() {
//        if(_results.size() > 0)
//        {
            result_type result = _results.front()->get_future().get();
            // _results.pop_front();
            return result;
//        }
//        else
//        {
//            return _future.get();
//        }
    }

    std::shared_ptr<ticket<T> > make_ticket()
    {
        auto _ticket = std::make_shared<ticket<T> >();
        _results.push_back(_ticket);
        return _ticket;
    }

protected:
//    std::atomic<bool> _cancel;
    std::list<std::shared_ptr<ticket<T> > > _results;
    // copyable
//    std::shared_future<result_type> _future;
//    // non-copyable
//    std::promise<result_type> _promise;
    std::jthread _task;
};


}
