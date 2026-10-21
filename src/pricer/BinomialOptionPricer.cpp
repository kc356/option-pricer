//
// Created by Kin on 10/21/2026.
//

#include <vector>
#include "../../include/pricer/BinomialOptionPricer.hpp"

double BinomialOptionPricer::price(Option& option, const MarketData& marketData) {
    std::vector<double> values(lattice_.steps + 1);
    for (int i { 0 }; i <= lattice_.steps; ++i) {
        const double spotPrice = marketData.spotPrice
            * std::pow(lattice_.up, i)
            * std::pow(lattice_.down, lattice_.steps - i);
        values[i] = option.payoff(spotPrice);
    }

    for (int i { lattice_.steps - 1 }; i >= 0; --i) {
        for (int j { 0 }; j <= i ; ++j) {
            values[j] = lattice_.discounting * (lattice_.riskNeutralProbability * values[j+1] + (1 - lattice_.riskNeutralProbability) * values[j]);
        }
    }

    return values[0];
}
