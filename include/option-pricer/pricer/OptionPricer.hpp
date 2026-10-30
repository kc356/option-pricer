//
// Created by Kin on 10/20/2026.
//

#ifndef OPTION_PRICER_OPTIONPRICER_HPP
#define OPTION_PRICER_OPTIONPRICER_HPP

#include <optional>

#include "../LatticeParam.hpp"
#include "../option/Option.hpp"

struct PricerResult {
    double price {};
    std::optional<double> standardError;
};

class OptionPricer {
public:
    virtual ~OptionPricer() = default;
    virtual PricerResult price(Option& option, const MarketData& marketData) = 0;
};

#endif // OPTION_PRICER_OPTIONPRICER_HPP