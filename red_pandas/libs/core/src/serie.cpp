#include "serie.h"
//
#include "sum.h"
#include "sub.h"
#include "mul.h"
#include "div.h"
#include <ta_libc.h>
#include "abs.h"
#include "operators/less.h"


struct talib_data
{
    talib_data()
    {
        TA_RetCode rc = TA_Initialize();
        if (rc != TA_SUCCESS)
            throw std::runtime_error("Error initializing TA Lib.");
    }

    ~talib_data() noexcept(false)
    {
        TA_RetCode rcshut = TA_Shutdown();
        if (rcshut != TA_SUCCESS)
            throw std::runtime_error("Error shutdown TA Lib ...");
    }
};

namespace talib_init {
    static talib_data init;
}


namespace rp {

    Serie ZERO(0.0);
/*
Serie HALF(0.5);
Serie ONE(1.0);
Serie TWO(2.0);
Serie PI(3.141592653589793238462643383279502884L);
Serie TWO_PI(2 * 3.141592653589793238462643383279502884L);
*/

}


// OPERATORS

std::shared_ptr<Serie> Serie::operator+(const std::shared_ptr<Serie>& other)
{
    return rp::sum2(shared_from_this(), other);
}

std::shared_ptr<Serie> Serie::operator-(const std::shared_ptr<Serie>& other)
{
    return rp::sub2(shared_from_this(), other);
}

std::shared_ptr<Serie> Serie::operator*(const std::shared_ptr<Serie>& other)
{
    return rp::mul2(shared_from_this(), other);
}

std::shared_ptr<Serie> Serie::operator/(const std::shared_ptr<Serie>& other)
{
    return rp::div2(shared_from_this(), other);
}

////////////////////////////////////////////////

Serie Serie::operator+(const Serie& other) const
{
    return rp::sum2ref(*this, other);
}

Serie Serie::operator-(const Serie& other) const
{
    return rp::sub2ref(*this, other);
}

Serie Serie::operator*(const Serie& other) const
{
    return rp::mul2ref(*this, other);
}

Serie Serie::operator/(const Serie& other) const
{
    return rp::div2(*this, other);
}

Serie Serie::operator-() const
{
    return rp::sub2ref(rp::ZERO, *this);
}
/////////////////////////////

std::shared_ptr<Serie> operator+(const std::shared_ptr<Serie>& one, const std::shared_ptr<Serie>& other)
{
    return rp::sum2(one, other);
}

std::shared_ptr<Serie> operator-(const std::shared_ptr<Serie>& one, const std::shared_ptr<Serie>& other)
{
    return rp::sub2(one, other);
}

std::shared_ptr<Serie> operator*(const std::shared_ptr<Serie>& one, const std::shared_ptr<Serie>& other)
{
    return rp::mul2(one, other);
}

std::shared_ptr<Serie> operator/(const std::shared_ptr<Serie>& one, const std::shared_ptr<Serie>& other)
{
    return rp::div2(one, other);
}

std::shared_ptr<Serie> operator-(const std::shared_ptr<Serie>& one)
{
    return rp::sub2(std::make_shared<Serie>(0.0), one);
}
/*
std::shared_ptr<Serie> operator+=(const std::shared_ptr<Serie>& one, const std::shared_ptr<Serie>& other)
{
    return rp::sum2(one, other);
}

std::shared_ptr<Serie> operator*=(const std::shared_ptr<Serie>& one, const std::shared_ptr<Serie>& other)
{
    return rp::mul2(one, other);
}
*/

bool operator==(const std::shared_ptr<Serie>& one, const std::shared_ptr<Serie>& other)
{
    rp::column_ptr left;
    if(one->type == SerieType::calculation_type)
        left = one->get_calc();
    else
        left = one;

    rp::column_ptr right;
    if(other->type == SerieType::calculation_type)
        right = other->get_calc();
    else
        right = other;

    return rp::all_less(
            rp::abs(left - right),
            std::make_shared<Serie>(std::forward<std::vector<double> >({1e-4}))
    );
}

std::ostream& operator<<(std::ostream &out, const std::shared_ptr<Serie>& s)
{
    out << s->__str__();
    return out;
}

std::ostream& operator<<(std::ostream &out, const Serie &s)
{
    out << s.__str__();
    return out;
}
