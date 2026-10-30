//
// Created by Kin on 10/25/2026.
//

#include "option-pricer/option/AmericanOption.hpp"

#include <algorithm>

double AmericanCallOption::payoff(double spotPrice) const {
    return std::max(spotPrice - strikePrice_, 0.0);
}

bool AmericanCallOption::canExercise(double currentTime) const {
    return true;
}

double AmericanPutOption::payoff(double spotPrice) const {
    return std::max(strikePrice_ - spotPrice, 0.0);
}

bool AmericanPutOption::canExercise(double currentTime) const {
    return true;
}
