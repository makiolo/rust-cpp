//
// Created by n424613 on 22/11/2022.
//

#ifndef RED_PANDAS_PROJECT_SYNC_H
#define RED_PANDAS_PROJECT_SYNC_H

#include <vector>
#include "method.h"
#include "connection.h"
// #include "clock.h"

namespace fes {

    template <typename... Args>
    class sync
    {
    public:
        using methods = methods_t<Args...>;

        explicit sync()
                : _registered()
                , _conns()
        {
            ;
        }

        template <typename T>
        inline weak_connection<Args...> connect(T* obj, void (T::*ptr_func)(const Args&...))
        {
            return _connect(obj, ptr_func, make_int_sequence<sizeof...(Args)>{});
        }

        template <typename METHOD>
        inline weak_connection<Args...> connect(METHOD&& fun)
        {
            typename methods::iterator it = _registered.emplace(_registered.end(), std::forward<METHOD>(fun));
            shared_connection<Args...> conn
                                               = std::make_shared<internal_connection<Args...>>(_registered, [it](methods& registered)
            {
                registered.erase(it);
            });
            _conns.push_back(conn);
            return weak_connection<Args...>(conn);
        }

        inline weak_connection<Args...> connect(sync<Args...>& callback)
        {
            return connect([&callback](const Args&... data)
                           {
                               callback(data...);
                           });
        }

        template <typename ... PARMS>
        void operator()(PARMS&&... data) const
        {
            auto it = _registered.begin();
            auto ite = _registered.end();
            for(; it != ite; ++it)
            {
                auto itee = --_registered.end();
                auto& reg = *it;
                if(it == itee) // is last iteration ?
                {
                    // forward
                    reg(std::forward<PARMS>(data)...);
                }
                else
                {
                    // copy
                    reg.call_copy(data...);
                }
            }
        }

    protected:
        template <typename T, int... Is>
        weak_connection<Args...> _connect(T* obj, void (T::*ptr_func)(const Args&...), int_sequence<Is...>)
        {
            typename methods::iterator it = _registered.emplace(
                    _registered.end(), std::bind(ptr_func, obj, placeholder_template<Is>()...));
            shared_connection<Args...> conn
                                               = std::make_shared<internal_connection<Args...> >(_registered, [it](methods& registered)
                    {
                        registered.erase(it);
                    });
            _conns.push_back(conn);
            return weak_connection<Args...>(conn);
        }

    protected:
        methods _registered;
        std::vector<shared_connection<Args...> > _conns;
    };

}

#endif //RED_PANDAS_PROJECT_SYNC_H
