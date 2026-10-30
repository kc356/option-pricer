//
// Created by Kin on 10/21/2026.
//

#include "option-pricer/pricer/BinomialOptionPricer.hpp"

#include <vector>

#include "option-pricer/pricer/OptionPricer.hpp"

PricerResult BinomialOptionPricer::price(Option& option, const MarketData& marketData) {
    std::vector<double> values(lattice_.steps + 1);
    for (int i {0}; i <= lattice_.steps; ++i) {
        const double spotPrice = marketData.spotPrice * std::pow(lattice_.up, i) *
                                 std::pow(lattice_.down, lattice_.steps - i);
        values[i] = option.payoff(spotPrice);
    }

    const double dt = option.getMaturity() / lattice_.steps;
    for (int i {lattice_.steps - 1}; i >= 0; --i) {
        const double currentTime = dt * i;
        const bool exercisable = option.canExercise(currentTime);
        for (int j {0}; j <= i; ++j) {
            double value =
                lattice_.discounting * (lattice_.riskNeutralProbability * values[j + 1] +
                                        (1 - lattice_.riskNeutralProbability) * values[j]);
            if (exercisable) {
                const double spotPrice = marketData.spotPrice * std::pow(lattice_.up, j) *
                                         std::pow(lattice_.down, i - j);
                value = std::max(value, option.payoff(spotPrice));
            }
            values[j] = value;
        }
    }

    return {values[0]};
}
