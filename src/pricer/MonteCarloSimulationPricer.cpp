//
// Created by Kin on 10/23/2026.
//


#include "option-pricer/pricer/MonteCarloSimulationPricer.hpp"

#include "option-pricer/pricer/OptionPricer.hpp"

PricerResult MonteCarloSimulationPricer::price(Option &option, const MarketData &marketData) {
    std::mt19937_64 engine(seed_);

    double sumPayoff {0.0}, sumPayoffSquared {0.0};
    for (long i {0}; i < paths_; ++i) {
        const double S_T = marketData.spotPrice * std::exp(
            (marketData.riskFreeRate - 0.5 * marketData.volatility * marketData.volatility) * option.getMaturity()
            + marketData.volatility * std::sqrt(option.getMaturity()) * normal_(engine));

        const double payoff = option.payoff(S_T);
        sumPayoff += payoff;
        sumPayoffSquared += payoff * payoff;
    }

    const double meanPayoff = sumPayoff / paths_;
    const double discountedMeanPayoff = meanPayoff * std::exp(-marketData.riskFreeRate * option.getMaturity());
    const double var = sumPayoffSquared / paths_ - meanPayoff * meanPayoff;

    return {discountedMeanPayoff, std::sqrt(var / paths_)};
}
