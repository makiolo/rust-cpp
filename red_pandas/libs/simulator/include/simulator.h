#ifndef RED_PANDAS_PROJECT_SIMULATOR_H
#define RED_PANDAS_PROJECT_SIMULATOR_H

#include <string>
#include <vector>
#include <unordered_map>
#include <entityx/entityx.h>
#include "serie.h"
#include "sum.h"
#include "portfolio.h"

namespace sim {

    class Simulator : public entityx::EntityX
    {
    public:
        enum Mode {
            PRICE_VOLATILITY,
            BACKTESTING,
        };
        using results_type = std::vector< std::unordered_map<std::string, rp::column_ptr> >;

        explicit Simulator(Mode mode);
        void append_path(const rp::column_ptr& price,
                         const rp::column_ptr& sigma,
                         const rp::column_ptr& mu);
        void update(const rp::column_ptr& dt);
        results_type& get_results();

    protected:
        void finish();

    protected:
        Mode mode;
        pf::BlockChain blockchain;
        results_type results;
        bool finished;
    };

};

#endif
