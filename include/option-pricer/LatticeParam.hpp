//
// Created by Kin on 10/21/2026.
//

#ifndef OPTION_PRICER_LATTICE_HPP
#define OPTION_PRICER_LATTICE_HPP
#include <cmath>

struct MarketData {
    double spotPrice;
    double riskFreeRate;
    double volatility;
    double dividendYield {0.0};
};

struct LatticeParam {
    double up;
    double down;
    double riskNeutralProbability;
    double discounting;
    int steps;
};

inline LatticeParam fromDiscrete(const double up, const double down, const double riskFreeRate,
                                 const int steps) {
    const double q = (riskFreeRate - down) / (up - down);
    return {up, down, q, 1.0 / riskFreeRate, steps};
}

inline LatticeParam CoxRossRubinstein(const double volatility, const double riskFreeRate,
                                     const double expiry, const int steps,
                                     const double dividendYield = 0.0) {
    const double dt = expiry / steps;
    const double up = std::exp(volatility * std::sqrt(dt));
    const double down = 1.0 / up;
    const double q = (std::exp((riskFreeRate - dividendYield) * dt) - down) / (up - down);
    return {up, down, q, std::exp(-riskFreeRate * dt), steps};
}

#endif // OPTION_PRICER_LATTICE_HPP