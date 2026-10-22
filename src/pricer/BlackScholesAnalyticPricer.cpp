//
// Created by Kin on 10/21/2026.
//

#include "../../include/BlackScholesAnalyticPricer.hpp"

#include <numbers>

double standardNormalCDF(const double x) {
    return 0.5 * std::erfc(-x * std::numbers::sqrt2_v<double> / 2.0);
}


double BlackScholesAnalyticPricer::price(Option &option, const MarketData &marketData) {
    const double d1 = (std::log(marketData.spotPrice / option.getStrikePrice())
        + (marketData.riskFreeRate + std::pow(marketData.volatility, 2) / 2) * option.getMaturity())
        / (marketData.volatility * std::sqrt(option.getMaturity()));

    const double d2 = d1 - marketData.volatility * std::sqrt(option.getMaturity());

    // C = SN(d1) - Ke^(-rT)N(d2)
    if (option.getOptionType() == OptionType::Put) {
        return option.getStrikePrice() * std::exp(-marketData.riskFreeRate * option.getMaturity()) * standardNormalCDF(-d2)
            - marketData.spotPrice * standardNormalCDF(-d1);
    }
    return marketData.spotPrice * standardNormalCDF(d1)
        - option.getStrikePrice() * std::exp(-marketData.riskFreeRate * option.getMaturity()) * standardNormalCDF(d2);
}
