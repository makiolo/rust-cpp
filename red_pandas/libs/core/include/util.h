#pragma once

#include <functional>
#include <type_traits>


template <typename T, typename U>
T get_value(U&& s0)
{
    T n0;

    if constexpr ( std::is_same_v<Serie, std::remove_cvref_t<U> > )
    {
        if (s0.type == SerieType::calculation_type)
        {
            s0.wait();
            n0 = s0.template get_result<T>();
        }
        else
        {
            n0 = s0.template get< T >();
        }
    }
    else
    {
        if (s0->type == SerieType::calculation_type)
        {
            s0->wait();
            n0 = s0->template get_result<T>();
        }
        else
        {
            n0 = s0->template get< T >();
        }
    }

    return n0;
}


template <typename U>
double* raw_data(U&& s0)
{
    if constexpr ( std::is_same_v<Serie, std::remove_cvref_t<U> > )
    {
        return s0.raw_data();
    }
    else
    {
        return s0->raw_data();
    }
}

template <typename U>
const double* const_raw_data(U&& s0)
{
    if constexpr ( std::is_same_v<Serie, std::remove_cvref_t<U> > )
    {
        return s0.raw_data();
    }
    else
    {
        return s0->raw_data();
    }
}

template <typename U>
size_t get_size(U&& s0)
{
    if constexpr ( std::is_same_v<Serie, std::remove_cvref_t<U> > )
    {
        return s0.size();
    }
    else
    {
        return s0->size();
    }
}

# undef _DEBUG
# include <Python.h>
# define _DEBUG 1

class gil_scoped_release {
public:
    explicit gil_scoped_release()
    {
        // tstate = PyEval_SaveThread();
    }

    ~gil_scoped_release() {
        /*
        if (!tstate) {
            return;
        }

        PyEval_RestoreThread(tstate);
        */
    }

private:
    PyThreadState* tstate;
};
