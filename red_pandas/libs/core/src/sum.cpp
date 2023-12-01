#include "sum.h"
//
#include <algorithm>
#include <future>
#include <thread>
#include <iostream>
#include "util.h"
#include "operators/isfinite.h"
#include "clean.h"
#include <unordered_set>
#include "mul.h"
#include "sub.h"
#include "pow.h"
#include "sqrt.h"
#include "max0.h"
#include "min0.h"
#include "cdf.h"

namespace rp {

    class Sum : public rp::Calculation<Serie> {
    public:
        DEFINE_KEY(Sum);

        template<typename T>
        explicit Sum(T&& s0, T&& s1) {
            _future = _promise.get_future();
            _task = std::thread([](std::promise<result_type>& promise, T&& ss0, T&& ss1) -> void {

#if defined(RELEASE_PYTHON_THREAD) && RELEASE_PYTHON_THREAD == 1
                gil_scoped_release release;
#endif
                auto s0 = rp::calculate(ss0);
                auto s1 = rp::calculate(ss1);
                const auto& n0 = get_value< Serie::Buffer >(s0);
                const auto& n1 = get_value< Serie::Buffer >(s1);

                if (n0.numCols() == n1.numCols()) {
                    promise.set_value(std::make_shared<Serie>(n0 + n1));
                } else if (n0.numCols() == 1) {
                    promise.set_value(std::make_shared<Serie>(n0[0] + n1));
                } else if (n1.numCols() == 1) {
                    promise.set_value(std::make_shared<Serie>(n0 + n1[0]));
                } else if (n0.numCols() > 1) {
                    throw std::runtime_error("TODO: Implement excel auto-fill");
                } else if (n1.numCols() > 1) {
                    throw std::runtime_error("TODO: Implement excel auto-fill");
                } else {
                    throw std::runtime_error("Invalid shapes on Sub.");
                }

            }, std::ref(_promise), std::forward<T>(s0), std::forward<T>(s1));
            _task.join();
        }
    };

    std::shared_ptr<Serie> sum2(const std::shared_ptr<Serie>& s0, const std::shared_ptr<Serie> &s1)
    {
        return std::make_shared<Serie>(Sum{s0, s1});
    }

    Serie sum2ref(const Serie& s0, const Serie& s1)
    {
        return Serie(Sum{s0, s1});
    }

    // TODO: MOVE CODE TO C++

    column_ptr array(const column& data)
    {
        return std::make_shared<column>(data);
    }

    column_ptr array(const std::initializer_list<double>& data)
    {
        return std::make_shared<column>(std::forward<internal_array>(data));
    }

    column_ptr array(const std::vector<double>& data)
    {
        return std::make_shared<column>(data);
    }

    column_ptr array(double* xx, int xx_n)
    {
        return std::make_shared<column>(xx, xx_n);
    }

    column_ptr array(double* xx, int xx_n, bool takeOwnerShip)
    {
        return std::make_shared<column>(xx, xx_n, takeOwnerShip);
    }

    rp::column_ptr agg_transpose(const dataframe& dataset, const function_ptr& aggregator)
    {
        if(dataset.empty()) return nullptr;

        if(dataset.size() == 1)
        {
            return dataset[0];
        }
        else
        {
            dataframe buffer;

            const size_t num_cols = dataset.size();

            size_t i = 0;
            while (i+1 < num_cols)
            {
                buffer.emplace_back(aggregator(dataset[i], dataset[i + 1]));
                i += 2;
            }
            if(i < num_cols)
            {
                buffer.emplace_back(dataset[i]);
            }
            return agg_transpose(buffer, aggregator);
        }
    }

    dataframe mapapply(const dataframe& dataset, const transformer_ptr& transformer)
    {
        dataframe newdf;
        for(const auto& col : dataset)
        {
            newdf.emplace_back(transformer(col));
        }
        return newdf;
    }

    dataframe mapcalculate(const dataframe& dataset)
    {
        dataframe newdf;
        bool somecalculate = false;
        for(const auto& col : dataset)
        {
            if(col->type == SerieType::calculation_type)
            {
                newdf.emplace_back(col->get_calc());
                somecalculate = true;
            }
            else
            {
                newdf.emplace_back(col);
            }
        }
        if (somecalculate)
            return newdf;
        else
            return dataset;
    }

    column_ptr mul(const dataframe& dataset)
    {
        auto result = agg(dataset, std::bind(rp::mul2, std::placeholders::_1, std::placeholders::_2));
        return result;
    }

    column_ptr mul_transpose(const dataframe& dataset)
    {
        auto result = agg_transpose(dataset, std::bind(rp::mul2, std::placeholders::_1, std::placeholders::_2));
        return result;
    }

    column_ptr sum(const dataframe& dataset)
    {
        auto result = agg(dataset, std::bind(rp::sum2, std::placeholders::_1, std::placeholders::_2));
        return result;
    }

    column_ptr sum_transpose(const dataframe& dataset)
    {
        auto result = agg_transpose(dataset, std::bind(rp::sum2, std::placeholders::_1, std::placeholders::_2));
        return result;
    }

    column_ptr mean(const dataframe& dataset, bool sample)
    {
        auto result = sum(dataset);
        if(sample)
            return result / rp::array({(double)dataset[0]->size() - 1.0});
        else
            return result / rp::array({(double)dataset[0]->size()});
    }

    column_ptr mean_transpose(const dataframe& dataset, bool sample)
    {
        auto result = sum_transpose(dataset);
        if(sample)
            return result / rp::array({(double)dataset.size() - 1.0});
        else
            return result / rp::array({(double)dataset.size()});
    }

    column_ptr max(const dataframe& dataset)
    {
        auto result = agg(dataset, std::bind(rp::max2, std::placeholders::_1, std::placeholders::_2));
        return result;
    }

    column_ptr max_transpose(const dataframe& dataset)
    {
        auto result = agg_transpose(dataset, std::bind(rp::max2, std::placeholders::_1, std::placeholders::_2));
        return result;
    }

    column_ptr min(const dataframe& dataset)
    {
        auto result = agg(dataset, std::bind(rp::min2, std::placeholders::_1, std::placeholders::_2));
        return result;
    }

    column_ptr min_transpose(const dataframe& dataset)
    {
        auto result = agg_transpose(dataset, std::bind(rp::min2, std::placeholders::_1, std::placeholders::_2));
        return result;
    }

    column_ptr var(const dataframe& dataset, bool sample)
    {
        auto mean1 = mean(dataset);
        auto transposed = transpose(dataset);
        return var_transpose(transposed, sample, mean1);
    }

    column_ptr var_transpose(const dataframe& dataset, bool sample, const column_ptr& mean0)
    {
        column_ptr mean1;
        if(mean0)
            mean1 = mean0; // var normal
        else
            mean1 = mean_transpose(dataset); // var transposed

        // dataframe result1 = rp::sub2(dataset, mean1);
        dataframe result1 = mapapply(dataset, std::bind(rp::sub2, std::placeholders::_1, mean1));
        // dataframe result2 = rp::pow(result1, rp::array({2.0}));
        dataframe result2 = mapapply(result1, std::bind(rp::pow, std::placeholders::_1, rp::array({2.0})));
        column_ptr result3 = mean_transpose(result2, sample);
        return result3;
    }

    column_ptr std(const dataframe& dataset, bool sample)
    {
        return rp::sqrt( var(dataset, sample) );
    }

    column_ptr std_transpose(const dataframe& dataset, bool sample)
    {
        return rp::sqrt( var_transpose(dataset, sample) );
    }

    dataframe transpose(const dataframe& datasetbase, bool drop_nan, double neutral, bool needcalculate)
    {
        if(datasetbase.empty()) return datasetbase;

        dataframe dataset;
        if (needcalculate)
            dataset = mapcalculate(datasetbase);
        else
            dataset = datasetbase;

        size_t num_rows;
        size_t num_cols;
        get_size(dataset, num_rows, num_cols);

        std::vector< std::vector<double> > matrix(num_rows, std::vector<double>(num_cols));
        std::unordered_set<size_t> drop_nans_blacklist_rows;

        for(size_t j = 0; j < num_cols; ++j)
        {
            const double* col_data = dataset[j]->raw_data();
            size_t col_size = dataset[j]->size();
            for(size_t i=0; i < col_size; ++i)
            {
                if(drop_nan) {
                    if (std::isfinite(col_data[i]))
                        matrix[i][j] = col_data[i];
                    else {
                        drop_nans_blacklist_rows.emplace(i);
                    }
                } else {
                    matrix[i][j] = col_data[i];
                }
            }
            for(size_t i=col_size; i < num_rows; ++i)
            {
                matrix[i][j] = neutral;
                drop_nans_blacklist_rows.emplace(i);
            }
        }

        dataframe new_dataset;

        for(size_t i = 0; i < num_rows; ++i)
        {
            if(drop_nan)
            {
                auto it = drop_nans_blacklist_rows.find(i);
                if (it == drop_nans_blacklist_rows.end())
                    new_dataset.emplace_back(rp::array(matrix[i]));
            } else {
                new_dataset.emplace_back(rp::array(matrix[i]));
            }
        }

        return new_dataset;
    }

    void get_size(const dataframe &dataset, size_t &num_rows, size_t &num_cols)
    {
        num_cols = dataset.size();
        num_rows = 0;
        for(size_t j = 0; j < num_cols; ++j)
        {
            size_t col_size = dataset[j]->size();
            if (col_size > num_rows) {
                num_rows = col_size;
            }
        }
    }

    void print(const column_ptr& column, const std::string& debug)
    {
        auto calculated = mapcalculate({column});

        if(!debug.empty())
        {
            std::cout << debug << std::endl << "---" << std::endl;
        }
        size_t num_rows = calculated[0]->size();
        size_t num_cols = 1;
        const double* column_data = calculated[0]->raw_data();
        for(size_t i = 0; i < num_rows; ++i)
        {
            for(size_t j = 0; j < num_cols; ++j)
            {
                std::cout << column_data[i] << "    ";
            }
            std::cout << std::endl;
        }
        std::cout << "." << std::endl;
    }

    void print(const dataframe& dataset, const std::string& debug)
    {
        auto calculated = mapcalculate(dataset);

        size_t num_rows;
        size_t num_cols;
        get_size(calculated, num_rows, num_cols);

        if(!debug.empty())
        {
            std::cout << debug << std::endl << "---" << std::endl;
        }

        for(size_t i = 0; i < num_rows; ++i)
        {
            for(size_t j = 0; j < num_cols; ++j)
            {
                std::cout << calculated[j]->raw_data()[i] << "    ";
            }
            std::cout << std::endl;
        }
        std::cout << "." << std::endl;
    }

    rp::column_ptr agg(const dataframe& dataset, const function_ptr& aggregator)
    {
        // check is normalized ?
        auto dataset_T = transpose(dataset, false, NAN);
        return agg_transpose(dataset_T, aggregator);
    }

    dataframe norm_nans(const dataframe& dataset)
    {
        auto dataset_T = transpose(dataset);
        return transpose(dataset_T);
    }

    dataframe norm_fill(const dataframe& dataset, double neutral)
    {
        auto dataset_T = transpose(dataset, false, neutral);
        return rp::transpose(dataset_T, false, neutral);
    }

    dataframe norm_drop(const dataframe& dataset)
    {
        auto dataset_T = transpose(dataset, true);
        return rp::transpose(dataset_T, true);
    }

    dataframe window(const column_ptr& data, int period)
    {
        /*
         * hacer slicing desplazando la ventana por el vector
         *
         * cada step genera un vector del tamaño window
         *
         * ahora sobre el dataframe podemos aplicar, mean, std ...
         */
        dataframe dataset;
        for(int i = 0; (i + period) < data->size(); ++i)
        {
            auto cutted = data->sub(i, i+period);
            dataset.emplace_back(cutted);
        }
        return dataset;
    }

    void use_callback(void (*f)(int i, const char* str))
    {
        f(100, "callback arg from c++");
    }

//    void example_unique(borrowed_ptr< Serie > borrowed)
//    {
//        //borrowed_ptr< std::unique_ptr<Serie> > borrowed(data);
//        std::cout << "size: = " << (*borrowed)->size() << std::endl;
//    }
//
}

extern "C" {
    void custommain() {

        auto s1 = rp::array({0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 2.0, 3.0});

        auto v = rp::calculate(rp::cdf(s1));

        auto vec = v->to_vector();
        for(size_t i = 0; i < vec.size(); ++i) {
            std::cout << i << " = " << vec[i] << std::endl;
        }
    }
}