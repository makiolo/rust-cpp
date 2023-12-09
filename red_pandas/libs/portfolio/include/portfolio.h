#ifndef RED_PANDAS_PROJECT_PORTFOLIO_H
#define RED_PANDAS_PROJECT_PORTFOLIO_H

#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <functional>

namespace pf {

    // const static double threshold = 1e-6;

    struct TxFrom
    {
        long txid;    // txid gastada
        long vout;  // slot gastado
    };

    struct TxTo
    {
        double amount; // cantidad disponible (TODO: use long long)
        std::string currency;
        long wallet;   // wallet que recibe el dinero
    };

    struct amount_info
    {
    public:
        explicit amount_info(double amount, std::string currency)
            : _amount(amount)
            , _currency(std::move(currency))
        {

        }

        double get_amount() const
        {
            return _amount;
        }

        const std::string& get_currency() const
        {
            return _currency;
        }

    protected:
        double _amount;
        std::string _currency;
    };

    struct Transaction
    {
        // id transaction
        long txid;
        // description
        std::string description;
        // group transaction
        std::string group;

        // pay identificator
        // std::string pay_id;
        // contract identificator
        // std::string contract_id;

        // translates to EUR, USD ...
        std::vector<amount_info> amounts;
        // Financiate from ...
        std::vector<TxFrom> froms;
        // Send to ...
        std::vector<TxTo> tos;

        double get_amount(const std::string& currency) {
            for(const auto& amount : amounts)
            {
                if(amount.get_currency() == currency)
                {
                    return amount.get_amount();
                }
            }
            std::cerr << "Invalid currency " << currency << std::endl;
            throw std::runtime_error("Invalid currency.");
        }
    };

    struct tuple_hash
    {
        std::size_t operator()(const std::tuple<long, long>& k) const
        {
            return std::get<0>(k) ^ std::get<1>(k);
        }
    };

    struct BlockChain  // Libro Contable
    {
        std::vector<Transaction> data;
        long lastid;
        mutable std::unordered_map< std::tuple<long, long>, std::tuple<Transaction, TxTo>, tuple_hash > cached_txtos;

        explicit BlockChain() : lastid(0)
        {

        }

        long nextid()
        {
            return lastid++;
        }

        bool is_utxo(const Transaction& transaction, const TxTo& to) const
        {
            for(size_t i = 0; i < data.size(); ++i)
            {
                const auto& trans = data[i];
                for(const auto& from : trans.froms)
                {
                    auto key = std::make_tuple(from.txid, from.vout);
                    const auto& [transaction_solved, txto_solved] = cached_txtos[key];
                    if (    (from.txid == transaction.txid) && \
                            (txto_solved.wallet == to.wallet) && \
                            (txto_solved.currency == to.currency) && \
                            (txto_solved.amount == to.amount))
                    {
                        return false;
                    }
                }
            }
            return true;
        }

        std::vector<std::tuple<Transaction, TxTo, size_t> > get_utxos(long wallet, double amount, const std::string& currency)
        {
            std::vector<std::tuple<Transaction, TxTo, size_t> > candidates;
            double total_amount = amount;
            for(auto& transaction : data)
            {
                size_t vout = 0;
                for(auto& to : transaction.tos)
                {
                    if(total_amount <= 1e-6)
                    {
                        break;
                    }
                    if(to.wallet == wallet && to.currency == currency)
                    {
                        if(is_utxo(transaction, to))
                        {
                            total_amount -= to.amount;
                            candidates.emplace_back(transaction, to, vout);
                        }
                    }
                    vout++;
                }
            }
            return candidates;
        }

        Transaction add_transaction(long from, long to,
                                    double amount, const std::string& currency,
                                    const std::string& description = "DEFAULT",
                                    const std::string& group = "DEFAULT",
                                    double amount_commission = 0.0,
                                    const std::string& currency_commission = "EUR",
                                    size_t precision = 1)
        {
            Transaction tx1;
            tx1.txid = nextid();
            tx1.amounts.emplace_back(amount, currency);
            tx1.description = description;
            tx1.group = group;

            if(from != 0)  // not god wallet
            {
                auto txtos = get_utxos(from, amount, currency);
                for(const auto& [transaction, txto, vout] : txtos)
                {
                    tx1.froms.emplace_back(transaction.txid, vout);
                }

                std::unordered_map<long, double> group_txtos;
                double spent_amount = amount;
                for(const auto& [transaction, txto, vout] : txtos)
                {
                    if (txto.amount > spent_amount)
                    {
                        // counter
                        if (group_txtos.find(to) == group_txtos.end())
                        {
                            group_txtos[to] = 0.0;
                        }
                        group_txtos[to] = group_txtos[to] + spent_amount;

                        double change = txto.amount - spent_amount;
                        if (change > 1e-6)
                        {
                            // counter
                            if (group_txtos.find(from) == group_txtos.end())
                            {
                                group_txtos[from] = 0.0;
                            }
                            group_txtos[from] = group_txtos[from] + change;
                        }
                        spent_amount -= spent_amount;
                    }
                    else
                    {
                        // counter
                        if (group_txtos.find(to) == group_txtos.end())
                        {
                            group_txtos[to] = 0.0;
                        }
                        group_txtos[to] = group_txtos[to] + txto.amount;

                        spent_amount -= txto.amount;
                    }
                    if(abs(spent_amount) <= 1e-6)
                    {
                        break;
                    }
                }

                size_t vout = 0;
                for(const auto& [k, v] : group_txtos)
                {
                    tx1.tos.emplace_back(v, currency, k);
                    cached_txtos[std::make_tuple(tx1.txid, vout)] = std::make_tuple(tx1, tx1.tos.back());
                    vout++;
                }

                if (abs(spent_amount) > 1e-6)
                {
                    throw std::runtime_error("Error transaction is not posible.");
                }
            }
            else
            {
                // god wallet create initial UTXOS
                tx1.tos.emplace_back(amount, currency, to);
                cached_txtos[std::make_tuple(tx1.txid, 0)] = std::make_tuple(tx1, tx1.tos.back());
            }

            if(!tx1.tos.empty())
                data.push_back(tx1);
            else
                throw std::runtime_error("Invalid transaction. TxTos are empty.");

            return tx1;
        }

        bool valid()
        {
            // check blockchain is owner
            return true;
        }

        std::unordered_map<std::string, double> balance(long wallet)
        {
            std::unordered_map<std::string, double> balances;
            for(auto& transaction : data)
            {
                for(auto& to : transaction.tos)
                {
                    if(to.wallet == wallet)
                    {
                        if (is_utxo(transaction, to))
                        {
                            if (balances.find(to.currency) == balances.end())
                            {
                                balances[to.currency] = 0.0;
                            }
                            balances[to.currency] += to.amount;
                        }
                    }
                }
            }
            return balances;
        }
    };

    ////////////////////////////////////

    struct Deal
    {
    public:
        // precio compra / precio venta
        virtual double price(const std::string& currency) = 0;
        // precio actual de mercado
        virtual double current_price(const std::string& currency) = 0;
        // valor de la compra
        virtual double buy_value(const std::string& currency) = 0;
        // gasto de comisiones en la compra
        virtual double comm_buy(const std::string& currency) = 0;
        // precio de adquisicion real
        virtual double adquisition(const std::string& currency) = 0;
        // valor de la venta
        virtual double sold_value(const std::string& currency) = 0;
        // gasto de comisiones en venta
        virtual double comm_sell(const std::string& currency) = 0;
        // precio de transmision real
        virtual double transmision(const std::string& currency) = 0;
        // beneficio bruto
        virtual double gross_profit(const std::string& currency) = 0;
        // beneficio neto
        virtual double net_profit(const std::string& currency) = 0;
        // rendimiento bruto
        virtual double gross_performance(const std::string& currency) = 0;
        // rendimiento neto
        virtual double net_performance(const std::string& currency) = 0;
    };

    struct OpenDeal : public Deal
    {
        Transaction buy_tx;

        double price(const std::string& currency)
        {
            // buy price ?
            return 0.0;
        }

        double current_price(const std::string& currency)
        {
            // get price from realtime
            // build currency from buy_tx.cuurency and sell_tx.currency
            return 0.0;
        }

        double buy_value(const std::string& currency)
        {
            return price(currency) * buy_tx.get_amount(currency);
        }

        double comm_buy(const std::string& currency)
        {
            // return buy_tx.get_amount_commission(currency);
            return 0.0;
        }

        double adquisition(const std::string& currency)
        {
            return buy_value(currency) + comm_buy(currency);
        }

        double sold_value(const std::string& currency)
        {
            return current_price(currency) * buy_tx.get_amount(currency);
        }

        double comm_sell(const std::string& currency)
        {
            // return buy_tx.get_amount_commission(currency);
            return 0.0;
        }

        double transmision(const std::string& currency)
        {
            return sold_value(currency) - comm_sell(currency);
        }

        double gross_profit(const std::string& currency)
        {
            return transmision(currency) - adquisition(currency);
        }

        double net_profit(const std::string& currency)
        {
            return sold_value(currency) - buy_value(currency);
        }

        double gross_performance(const std::string& currency)
        {
            return (transmision(currency) / adquisition(currency)) - 1.0;
        }

        double net_performance(const std::string& currency)
        {
            return (sold_value(currency) / buy_value(currency)) - 1.0;
        }
    };

    struct ClosedDeal : public Deal
    {
        OpenDeal open_deal;
        Transaction sell_tx;

        double price(const std::string& currency)
        {
            return open_deal.buy_tx.get_amount(currency) / sell_tx.get_amount(currency);
        }

        double current_price(const std::string& currency)
        {
            // get price from realtime
            // build currency from buy_tx.cuurency and sell_tx.currency
            return 0.0;
        }

        double buy_value(const std::string& currency) {
            return price(currency) * open_deal.buy_tx.get_amount(currency);
        }

        double comm_buy(const std::string& currency) {
            // return open_deal.buy_tx.get_amount_commission(currency);
            return 0.0;
        }

        double adquisition(const std::string& currency)
        {
            return buy_value(currency) + comm_buy(currency);
        }

        double sold_value(const std::string& currency)
        {
            return current_price(currency) * sell_tx.get_amount(currency);
        }

        double comm_sell(const std::string& currency)
        {
            // return sell_tx.get_amount_commission(currency);
            return 0.0;
        }

        double transmision(const std::string& currency)
        {
            return sold_value(currency) - comm_sell(currency);
        }

        double gross_profit(const std::string& currency)
        {
            return transmision(currency) - adquisition(currency);
        }

        double net_profit(const std::string& currency)
        {
            return sold_value(currency) - buy_value(currency);
        }

        double gross_performance(const std::string& currency)
        {
            return (transmision(currency) / adquisition(currency)) - 1.0;
        }

        double net_performance(const std::string& currency)
        {
            return (sold_value(currency) / buy_value(currency)) - 1.0;
        }
    };

    /*
    struct Portfolio
    {
        std::vector<Deal> deals;
    };
    */

    struct SymbolInfoTick
    {
        double ask;
        double bid;
    };

    struct FakeTrader
    {
        // initialize(path)
        // login(login, password, server)
        // terminal_info()
        // version()

        //// open positions
        // positions_get()
        // positions_get(symbol)
        // positions_total()

        // history_deals_get(from, to, group=symbol)
        // history_orders_get(from, to, group=symbol)

        //// pending positions
        // orders_get()
        // orders_get(symbol)
        // orders_total()

        // order_calc_margin(Order, symbol, volume, price_open)
        // order_calc_profit(Orderm symbol, volume, price_open, tp)

        // symbol_info_tick(symbol)

        // order_send()

        enum Order
        {
            ORDER_TYPE_BUY = 0,
            ORDER_TYPE_SELL
        };
    };

};

#endif
