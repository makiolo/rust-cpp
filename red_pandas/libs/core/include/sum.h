#pragma once

#include "serie.h"

namespace rp {

    // Python
    std::shared_ptr<Serie> sum2(const std::shared_ptr<Serie>& s0, const std::shared_ptr<Serie> &s1);

    // C++
    Serie sum2ref(const Serie& s0, const Serie& s1);

    // make reactive
    std::shared_ptr<Serie> sum_reactive(const std::shared_ptr<Serie>& s0, const std::shared_ptr<Serie> &s1);

    // TODO: MOVE CODE TO C++ CORE

    /*
    column_ptr divide_count(const column_ptr& result)
    {
        return result / rp::array({(double)result->size()});
    }

    column_ptr divide_count_minus_one(const column_ptr& result)
    {
        return result / rp::array({(double)result->size() - 1});
    }
    */

    // cumsum
    // cummul
    // sum (sum2 + null)
    // max (max + null)
    // min (min + null)
    // mean (sum2 + divide_count)
    // std ()
    // var ()
    // cummax ??
    // corr (matriz de correlaciones)
    // cov (matriz de covarianzas)

    column_ptr first(const column_ptr& left, const column_ptr& right);
    column_ptr last(const column_ptr& left, const column_ptr& right);

    column_ptr agg(const dataframe& dataset, const function_ptr& aggregator);
    column_ptr agg_transpose(const dataframe& dataset, const function_ptr& aggregator);

    dataframe mapapply(const dataframe& dataset, const transformer_ptr& transformer);
    dataframe mapcalculate(const dataframe& dataset);

    column_ptr sum(const dataframe& dataset);
    column_ptr sum_transpose(const dataframe& dataset);

    column_ptr mul(const dataframe& dataset);
    column_ptr mul_transpose(const dataframe& dataset);

    column_ptr mean(const dataframe& dataset, bool sample = false);
    column_ptr mean_transpose(const dataframe& dataset, bool sample = false);

    column_ptr max(const dataframe& dataset);
    column_ptr max_transpose(const dataframe& dataset);

    column_ptr min(const dataframe& dataset);
    column_ptr min_transpose(const dataframe& dataset);

    column_ptr var(const dataframe& dataset, bool sample = true);
    column_ptr var_transpose(const dataframe& dataset, bool sample = true, const column_ptr& mean0 = nullptr);

    column_ptr std(const dataframe& dataset, bool sample = true);
    column_ptr std_transpose(const dataframe& dataset, bool sample = true);

    dataframe window(const column_ptr& data, int period, bool right = true);

    // window agg()
    column_ptr window2(const column_ptr& data, int period, const function_ptr& aggregator);
    column_ptr window2_mean(const column_ptr& data, int period, bool sample = false);

    // window indicators
    dataframe window3(const dataframe& data, int period, const indicator_ptr& indicator);
    column_ptr window3_last_rsi(const column_ptr& close, int window_period = 14, int rsi_period = 14);


    dataframe transpose(const dataframe& dataset, bool drop_nan = false, double neutral = NAN, bool needcalculate = true);

    // cumsum
    // dataframe rolling(const dataframe& dataset, int window, int min_periods=1);
    // dataframe ewm(const dataframe& dataset, double com, double span, double halflife);
    // expanding ?? incrmeental window size from 0 (very similar to rolling)

    void get_size(const dataframe &dataset, size_t &num_rows, size_t &num_cols);
    void print(const dataframe& dataset, const std::string& debug="");
    void print(const column_ptr& column, const std::string& debug="");

    dataframe norm_nans(const dataframe& dataset);
    dataframe norm_fill(const dataframe& dataset, double neutral = 0.0);
    dataframe norm_drop(const dataframe& dataset);

    // horizontal or vertical ?
    /*
    dataframe sort(const dataframe& dataset, const std::vector<int>& keys = {0, })
    {
        return dataframe();
    }
    */

    // sort
    // sort transpose

    // group_by
    // group_by_transpose

    // slicing multi dimensional ?

    void use_callback(void (*f)(int i, const char* str));

}

extern "C" {
    void custommain();
}
