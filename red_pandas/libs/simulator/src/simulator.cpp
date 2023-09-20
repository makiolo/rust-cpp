#include "simulator.h"
//
#include <random>
#include <utility>
#include "exp.h"
#include "pow.h"
#include "sum.h"
#include "sqrt.h"
#include "rand_normal.h"
#include "formulas/greeks.h"
#include "rsi.h"
#include "clean.h"

namespace sim {

static int asset_id = 0;

struct Asset
{
    explicit Asset(rp::column_ptr  price, rp::column_ptr  sigma, rp::column_ptr  mu)
            : id(asset_id++)
            , price(std::move(price))
            , sigma(std::move(sigma))
            , mu(std::move(mu))
    {
        ;
    }

    int id;
    rp::dataframe historic_price;
    rp::dataframe historic_sigma;
    //
    rp::column_ptr price;
    rp::column_ptr sigma;
    rp::column_ptr mu;
};

class PriceSystem : public entityx::System<PriceSystem>
{
public:
    void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override
    {
        es.each<Asset>([&](entityx::Entity entity, Asset &asset)
        {
            auto new_price = asset.price * rp::exp(
                (asset.mu - rp::pow(asset.sigma, rp::two()) / rp::two()) * dt + (asset.sigma * rp::sqrt(dt) * rp::rand_normal(asset.price->size()))
            );
            asset.historic_price.emplace_back(new_price);
            asset.price = asset.historic_price.back();
        });
    };
};

class VolatilitySystem : public entityx::System<VolatilitySystem>
{
public:
    void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override
    {
        es.each<Asset>([&](entityx::Entity entity, Asset &asset)
        {
            auto new_sigma = asset.sigma * rp::exp(
                ((rp::constant(3.0) * rp::rand_normal(asset.sigma->size())) - rp::pow(asset.sigma, rp::two()) / rp::two()) * dt + (asset.sigma * rp::sqrt(dt) * rp::rand_normal(asset.sigma->size()))
            );
            asset.historic_sigma.emplace_back(new_sigma);
            asset.sigma = new_sigma;
        });
    };
};

Simulator::Simulator(Mode mode)
    : finished(false)
    , mode(mode)
{
    switch(mode)
    {
        case BACKTESTING:
            throw std::runtime_error("not implemmented yet.");
        case PRICE_VOLATILITY:
        default:
            systems.add<PriceSystem>();
            //systems.add<VolatilitySystem>();
    }
    systems.configure();
}

void Simulator::append_path(const rp::column_ptr& price, const rp::column_ptr& sigma, const rp::column_ptr& mu)
{
    entityx::Entity entity = entities.create();
    auto asset = entity.assign<Asset>(price, sigma, mu);

    /*
     * testear cobertura
     *
     * cuenta opciones 300000
     * cuenta acciones 200000
     * cuenta cash ($) 100000
     */
    blockchain.add_transaction(0, 100000 + asset->id, 5000, "$", "Initial account");
}

void Simulator::update(const rp::column_ptr& dt) {
    if(finished)
    {
        results.clear();
        finished = false;
    }
    switch(mode)
    {
        case BACKTESTING:
            throw std::runtime_error("not implemmented yet.");
        case PRICE_VOLATILITY:
        default:
            systems.update<PriceSystem>(dt);
            // systems.update<VolatilitySystem>(dt);
    }
}

void Simulator::finish()
{
    entities.each<sim::Asset>([&](entityx::Entity entity, sim::Asset &asset) {

        std::unordered_map<std::string, rp::column_ptr> path;

        path["price"] = asset.historic_price.back();
        // path["sigma"] = asset.historic_sigma.back();

        auto transposed = rp::transpose(asset.historic_price);

        for(size_t i = 0; i < transposed.size(); ++i)
        {
            std::stringstream ss;
            ss << "price_" << i;
            path[ss.str()] = transposed[i];
            //
            std::stringstream ss2;
            ss2 << "rsi_" << i;
            path[ss2.str()] = rp::clean(rp::rsi(transposed[i]));
            // where criteria

            // diff criteria, create signal

            // convert signals in buy / sell

        }

        // miramos el balance en cada timestep
        // el resultado es un vector<double> con el balance
        for(const auto& [a, b] : blockchain.balance(100000 + asset.id))
        {
            std::cout << "id = " << asset.id << ", a = " << a << ", b = " << b << std::endl;
        }

        // generar todas las combinaciones "linspace" para todas las variables
        // de las que depende la decision
        //
        // crear una signal por cada RSI (12 signals (strike y tiempo) X paths)

        results.emplace_back(path);
        std::cout << ".";
    });
    std::cout << std::endl;
    finished = true;
}

Simulator::results_type& Simulator::get_results()
{
    if(!finished)
        finish();
    return results;
}

}
