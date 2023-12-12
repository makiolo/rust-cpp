//
// Created by n424613 on 11/12/2023.
//

#ifndef RED_PANDAS_PROJECT_ITERABLEREPOSITORY_H
#define RED_PANDAS_PROJECT_ITERABLEREPOSITORY_H

struct NumericalDimension
{
    NumericalDimension(double chunk_size, double distance_far)
    {

    }
};

template <typename ...Dimensions>
class IterableRepository
{
public:
    explicit IterableRepository(Dimensions&& ... dimensions)
    {

    }
};

#endif //RED_PANDAS_PROJECT_ITERABLEREPOSITORY_H
