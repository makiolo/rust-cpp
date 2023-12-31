//
// Created by n424613 on 22/11/2022.
//

#ifndef RED_PANDAS_PROJECT_INTERNAL_CONNECTION_H
#define RED_PANDAS_PROJECT_INTERNAL_CONNECTION_H

#include <atomic>
#include <functional>
#include "method.h"

namespace fes {

    template <typename... Args>
    class internal_connection
    {
    public:
        using deleter_t = std::function<void(methods_t<Args...>&)>;

        // use implicit "explicitly" :D
        internal_connection(methods_t<Args...>& registered, const deleter_t& deleter)
                : _deleter(deleter)
                , _connected(true)
                , _registered(registered)
        { ; }

        ~internal_connection() { ; }
        internal_connection(const internal_connection& other) = delete;
        internal_connection& operator=(const internal_connection& other) = delete;

        void disconnect()
        {
            if (_connected)
            {
                _deleter(_registered);
                _connected = false;
            }
        }

    protected:
        deleter_t _deleter;
        std::atomic<bool> _connected;
        methods_t<Args...>& _registered;
    };

}

#endif //RED_PANDAS_PROJECT_INTERNAL_CONNECTION_H
