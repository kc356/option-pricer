//
// Created by Kin on 10/25/2026.
//


#include "option-pricer/option/EuropeanOption.hpp"

#include <algorithm>

double EuropeanCallOption::payoff(const double spotPrice) const {
    return std::max(spotPrice - strikePrice_, 0.0);
}

bool EuropeanCallOption::canExercise(const double currentTime) const {
    return currentTime >= maturity_;
}

double EuropeanPutOption::payoff(const double spotPrice) const {
    return std::max(strikePrice_ - spotPrice, 0.0);
}

bool EuropeanPutOption::canExercise(const double currentTime) const {
    return currentTime >= maturity_;
}
