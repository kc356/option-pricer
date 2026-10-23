//
// Created by Kin on 10/21/2026.
//

#ifndef OPTION_PRICER_BLACKSCHOLESANALYTICPRICER_HPP
#define OPTION_PRICER_BLACKSCHOLESANALYTICPRICER_HPP
#include "OptionPricer.hpp"


class BlackScholesAnalyticPricer final : public OptionPricer{
public:
    PricerResult price(Option &option, const MarketData &marketData) override;
};


#endif //OPTION_PRICER_BLACKSCHOLESANALYTICPRICER_HPP