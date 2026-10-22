//
// Created by Kin on 10/21/2026.
//

#ifndef OPTION_PRICER_BLACKSCHOLESANALYTICPRICER_HPP
#define OPTION_PRICER_BLACKSCHOLESANALYTICPRICER_HPP
#include "pricer/OptionPricer.hpp"


class BlackScholesAnalyticPricer final : public OptionPricer{
public:
    double price(Option &option, const MarketData &marketData) override;

    using OptionPricer::price;
};


#endif //OPTION_PRICER_BLACKSCHOLESANALYTICPRICER_HPP