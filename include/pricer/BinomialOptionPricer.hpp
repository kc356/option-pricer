//
// Created by Kin on 10/21/2026.
//

#ifndef OPTION_PRICER_BINOMIALOPTIONPRICER_HPP
#define OPTION_PRICER_BINOMIALOPTIONPRICER_HPP
#include "OptionPricer.hpp"


class BinomialOptionPricer final : public OptionPricer {
public:

    explicit BinomialOptionPricer(const LatticeParam& lattice)
        : lattice_(lattice) {};

    double price(Option& option, const MarketData& marketData) override;

private:
    LatticeParam lattice_;
};

#endif //OPTION_PRICER_BINOMIALOPTIONPRICER_HPP