//
// Created by Kin on 10/21/2026.
//

#ifndef OPTION_PRICER_BLACKSCHOLESANALYTICPRICER_HPP
#define OPTION_PRICER_BLACKSCHOLESANALYTICPRICER_HPP
#include "option-pricer/option/AmericanOption.hpp"

#include "OptionPricer.hpp"

class BlackScholesAnalyticPricer final : public OptionPricer {
public:
    PricerResult price(Option& option, const MarketData& marketData) override;
    PricerResult price(AmericanCallOption& option, const MarketData& marketData) = delete;
    PricerResult price(AmericanPutOption& option, const MarketData& marketData) = delete;
};

#endif // OPTION_PRICER_BLACKSCHOLESANALYTICPRICER_HPP