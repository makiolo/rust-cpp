//
// Created by n424613 on 16/11/2022.
//

#ifndef RED_PANDAS_PROJECT_SERIE2D_H
#define RED_PANDAS_PROJECT_SERIE2D_H

#include <variant>
#include <memory>
#include <NumCpp.hpp>

namespace rp {

class Serie2d : public std::enable_shared_from_this<Serie2d>
{
public:
    Serie2d() = default;
    ~Serie2d() = default;

    Serie2d(const Serie2d &other)
            : values(other.values)
    {
        //std::cout << "copy constructor" << std::endl;
    }

    Serie2d(Serie2d &&other) noexcept
            : values(std::move(other.values))
    {
        //std::cout << "move constructor" << std::endl;
    }

    Serie2d &operator=(const Serie2d &other)
    {
        //std::cout << "operator= copy" << std::endl;
        values = other.values;
        return *this;
    }

    Serie2d &operator=(Serie2d &&other) noexcept
    {
        //std::cout << "operator= move" << std::endl;
        values = std::move(other.values);
        return *this;
    }

    explicit Serie2d(double *xx, int xx_n, int yy_n)
            : values(Serie::Buffer(xx, xx_n, yy_n))
    {
        std::cout << "2d matrix" << std::endl;
    }

protected:
    std::variant< Serie::Buffer > values;
};

}

#endif //RED_PANDAS_PROJECT_SERIE2D_H
