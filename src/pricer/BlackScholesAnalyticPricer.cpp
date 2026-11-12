//
// Created by Kin on 10/21/2026.
//

#include "option-pricer/pricer/BlackScholesAnalyticPricer.hpp"

#include <cmath>
#include <numbers>

#include "option-pricer/pricer/OptionPricer.hpp"

double standardNormalCDF(const double x) {
    return 0.5 * std::erfc(-x * std::numbers::sqrt2_v<double> / 2.0);
}

PricerResult BlackScholesAnalyticPricer::price(Option& option, const MarketData& marketData) {
    const double T = option.getMaturity();
    const double K = option.getStrikePrice();
    const double sigmaSqrtT = marketData.volatility * std::sqrt(T);

    const double d1 = (std::log(marketData.spotPrice / K) +
                       (marketData.riskFreeRate - marketData.dividendYield +
                        0.5 * marketData.volatility * marketData.volatility) *
                           T) /
                      sigmaSqrtT;
    const double d2 = d1 - sigmaSqrtT;

    const double discountedSpot = marketData.spotPrice * std::exp(-marketData.dividendYield * T);
    const double discountedStrike = K * std::exp(-marketData.riskFreeRate * T);

    double price;
    if (option.getOptionType() == OptionType::Put) {
        price = discountedStrike * standardNormalCDF(-d2) - discountedSpot * standardNormalCDF(-d1);
    } else {
        price = discountedSpot * standardNormalCDF(d1) - discountedStrike * standardNormalCDF(d2);
    }

    return {price};
}
