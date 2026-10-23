//
// Created by Kin on 10/22/2026.
//

#ifndef OPTION_PRICER_FINITEDIFFERENCEPRICER_HPP
#define OPTION_PRICER_FINITEDIFFERENCEPRICER_HPP
#include "OptionPricer.hpp"


class FiniteDifferencePricer final : public OptionPricer {
public:
    double price(Option &option, const MarketData &marketData) override;

    FiniteDifferencePricer(const int timeSteps, const int priceSteps)
        : timeSteps_(timeSteps), priceSteps_(priceSteps) {};

private:
    int timeSteps_ {};
    int priceSteps_ {};
};


#endif //OPTION_PRICER_FINITEDIFFERENCEPRICER_HPP