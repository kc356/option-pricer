//
// Created by Kin on 10/19/2026.
//

#include "../include/Option.hpp"
#include <algorithm>

double EuropeanCallOption::payoff(const double spotPrice){
    return std::max(spotPrice - strikePrice_, 0.0);
}

double EuropeanPutOption::payoff(const double spotPrice){
    return std::max(strikePrice_ - spotPrice, 0.0);
}