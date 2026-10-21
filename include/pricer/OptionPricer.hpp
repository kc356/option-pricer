//
// Created by Kin on 10/20/2026.
//

#ifndef OPTION_PRICER_OPTIONPRICER_HPP
#define OPTION_PRICER_OPTIONPRICER_HPP

#include "../LatticeParam.hpp"
#include "../Option.hpp"


class OptionPricer {
public:
    virtual ~OptionPricer() = default;
    virtual double price(Option& option, const MarketData& marketData) = 0;
};

#endif //OPTION_PRICER_OPTIONPRICER_HPP