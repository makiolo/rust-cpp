#include <catch_amalgamated.hpp>
#include "portfolio.h"
#include "npv.h"

int dice3 (void) {
    return rand() % 3 + 1;
}

TEST_CASE("portfolio_test1", "[example]") {

    pf::BlockChain blockchain;

    long oldpos;
    long newpos = 0;
    int i = 0;
    while(i < 100)
    {
        do {
            oldpos = newpos;
            newpos = dice3();
        } while(oldpos == newpos);
        blockchain.add_transaction(oldpos, newpos, 1, "bola", "Transfer");
        ++i;
    }

    for(const auto& wallet : {1, 2, 3})
    {
        for(const auto& [asset, balance] : blockchain.balance(wallet))
        {
            REQUIRE(balance == Catch::Approx(1));
        }
    }
    std::cout << "ok" << std::endl;
}

TEST_CASE("future flows", "[wallets]") {

    std::vector<std::tuple<int, int, double, std::string> > flows;
    // origen dinero, destino dinero y cantidad (sin tener en cuenta fecha)
    flows.emplace_back(std::make_tuple(0, 1, 1000.0, "EUR"));
    flows.emplace_back(std::make_tuple(1, 2, 1000.0, "EUR"));
    flows.emplace_back(std::make_tuple(0, 1, 1000.0, "EUR"));
    flows.emplace_back(std::make_tuple(1, 2, 1000.0, "EUR"));
    flows.emplace_back(std::make_tuple(0, 1, 1000.0, "EUR"));
    flows.emplace_back(std::make_tuple(1, 2, 1000.0, "EUR"));
    flows.emplace_back(std::make_tuple(0, 1, 1000.0, "EUR"));
    flows.emplace_back(std::make_tuple(1, 2, 1000.0, "EUR"));
    flows.emplace_back(std::make_tuple(2, 1, 500.0, "EUR"));

    pf::BlockChain blockchain;
    for(const auto& [start, end, flow, asset] : flows)
    {
        blockchain.add_transaction(start, end, flow, asset);
    }

    for(const auto& wallet : {1, 2, 3})
    {
        for(const auto& [asset, balance] : blockchain.balance(wallet))
        {
            std::cout << "balance " << wallet << ": " << balance << " " << asset << std::endl;
        }
    }
}

TEST_CASE("future flows estimated with curve", "[wallets]") {

    qs::TermStructure term1;
    term1.append_spot(-2, qs::InterestRate(0.02, qs::Convention::YIELD, qs::Frequency::ANNUAL));
    term1.append_spot(0, qs::InterestRate(0.05, qs::Convention::YIELD, qs::Frequency::ANNUAL));
    term1.append_spot(1, qs::InterestRate(0.01, qs::Convention::YIELD, qs::Frequency::ANNUAL));
    term1.append_spot(2, qs::InterestRate(0.02, qs::Convention::YIELD, qs::Frequency::ANNUAL));
    term1.append_spot(3, qs::InterestRate(0.01, qs::Convention::YIELD, qs::Frequency::ANNUAL));
    term1.build();

    std::vector<std::tuple<int, int, qs::CustomCashFlow, std::string> > flows;
    // tiempos relativos desde la fecha de evaluación
    // flujos pasados valen lo que valen
    // origen dinero, destino dinero, y fecha de la transacción futura (flujo futuro, estimado con la curva)

    enum Accounts
    {
        EXTERNAL_WORLD = 0,
        PERSONAL,
        FRIEND,
    };

    // Flujos pasados, preguntar a Hector

    // Los Asset tienen "precision" (double máximo 15, float 9)

    flows.emplace_back(std::make_tuple(EXTERNAL_WORLD, PERSONAL, term1.make_cashflow(0.0, 1.0), "VACA"));
    flows.emplace_back(std::make_tuple(PERSONAL, FRIEND, term1.make_cashflow(1.0, 1.0), "VACA"));
    flows.emplace_back(std::make_tuple(0, 1, term1.make_cashflow(0.0, 1000.0), "EUR"));
    flows.emplace_back(std::make_tuple(1, 2, term1.make_cashflow(0.0, 1000.0), "EUR"));
    flows.emplace_back(std::make_tuple(0, 1, term1.make_cashflow(1.0, 1000.0), "EUR"));
    flows.emplace_back(std::make_tuple(1, 2, term1.make_cashflow(1.0, 1000.0), "EUR"));
    flows.emplace_back(std::make_tuple(0, 1, term1.make_cashflow(2.0, 1000.0), "EUR"));
    flows.emplace_back(std::make_tuple(1, 2, term1.make_cashflow(2.0, 1000.0), "EUR"));
    flows.emplace_back(std::make_tuple(0, 1, term1.make_cashflow(3.0, 1000.0), "EUR"));
    flows.emplace_back(std::make_tuple(1, 2, term1.make_cashflow(3.0, 1000.0), "EUR"));
    flows.emplace_back(std::make_tuple(2, 1, term1.make_cashflow(1.5, 500.0), "EUR"));

    pf::BlockChain blockchain;
    for(const auto& [start, end, flow, asset] : flows)
    {
        blockchain.add_transaction(start, end, flow.to_start_cashflow().cash, asset);
    }

    for(const auto& wallet : {1, 2, 3})
    {
        for(const auto& [asset, balance] : blockchain.balance(wallet))
        {
            std::cout << "balance " << wallet << ": " << balance << " " << asset << std::endl;
        }
    }
}
