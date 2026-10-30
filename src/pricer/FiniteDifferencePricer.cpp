//
// Created by Kin on 10/22/2026.
//

#include "option-pricer/pricer/FiniteDifferencePricer.hpp"

#include <vector>

#include "option-pricer/pricer/OptionPricer.hpp"

PricerResult FiniteDifferencePricer::price(Option& option, const MarketData& marketData) {
    const double dt = option.getMaturity() / timeSteps_;
    const double dS = 4.0 * marketData.spotPrice / priceSteps_;

    std::vector<double> values(priceSteps_ + 1), newValues(priceSteps_ + 1);
    for (int i {0}; i <= priceSteps_; ++i) {
        values[i] = option.payoff(i * dS);
    }

    for (int j {0}; j < timeSteps_; ++j) {
        for (int i {1}; i < priceSteps_; ++i) {
            const double s2i2 = marketData.volatility * marketData.volatility * i * i;
            const double a = 0.5 * dt * (s2i2 - marketData.riskFreeRate * i);
            const double b = 1.0 - dt * (s2i2 + marketData.riskFreeRate);
            const double c = 0.5 * dt * (s2i2 + marketData.riskFreeRate * i);
            newValues[i] = a * values[i - 1] + b * values[i] + c * values[i + 1];
        }

        newValues[0] = values[0] * (1.0 - marketData.riskFreeRate * dt);
        newValues[priceSteps_] = 2.0 * newValues[priceSteps_ - 1] - newValues[priceSteps_ - 2];

        if (option.canExercise(option.getMaturity() - (j + 1) * dt)) {
            for (int i {0}; i < priceSteps_; ++i) {
                newValues[i] = std::max(newValues[i], option.payoff(i * dS));
            }
        }

        std::swap(values, newValues);
    }

    const double x = marketData.spotPrice / dS;
    const int i = static_cast<int>(x);
    const double w = x - i;
    return {(1.0 - w) * values[i] + w * values[i + 1]};
}
