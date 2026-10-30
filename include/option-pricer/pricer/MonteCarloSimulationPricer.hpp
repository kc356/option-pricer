//
// Created by Kin on 10/23/2026.
//

#ifndef OPTION_PRICER_MONTECARLOSIMULATIONPRICER_HPP
#define OPTION_PRICER_MONTECARLOSIMULATIONPRICER_HPP
#include <random>

#include "option-pricer/option/AmericanOption.hpp"

#include "OptionPricer.hpp"

class MonteCarloSimulationPricer final : public OptionPricer {
public:
    PricerResult price(Option& option, const MarketData& marketData) override;

    PricerResult price(AmericanCallOption& option, const MarketData& marketData) = delete;
    PricerResult price(AmericanPutOption& option, const MarketData& marketData) = delete;

    explicit MonteCarloSimulationPricer(const long paths, const long long seed)
        : seed_(seed), paths_(paths) {};

private:
    long long seed_ {};
    long paths_ {};
    std::normal_distribution<> normal_ {0.0, 1.0};
};

#endif // OPTION_PRICER_MONTECARLOSIMULATIONPRICER_HPP