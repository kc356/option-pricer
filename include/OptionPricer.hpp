//
// Created by Kin on 10/20/2026.
//

#ifndef OPTION_PRICER_OPTIONPRICER_HPP
#define OPTION_PRICER_OPTIONPRICER_HPP

#include <cmath>
#include <vector>
#include "Lattice.hpp"
#include "Option.hpp"


class OptionPricer {
public:
    virtual ~OptionPricer() = default;
    virtual double price(Option& option, const MarketData& marketData) = 0;
};


class BinomialOptionPricer final : public OptionPricer {
    public:

    explicit BinomialOptionPricer(const Lattice& model)
        : lattice_(model) {};

    double price(Option& option, const MarketData& marketData) override;

private:
    Lattice lattice_;
};

#endif //OPTION_PRICER_OPTIONPRICER_HPP