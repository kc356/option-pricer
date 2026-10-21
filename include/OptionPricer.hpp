//
// Created by Kin on 10/20/2026.
//

#ifndef OPTION_PRICER_OPTIONPRICER_HPP
#define OPTION_PRICER_OPTIONPRICER_HPP

#include <cmath>
#include <vector>
#include "Option.hpp"

struct MarketData {
    double spotPrice;
    double riskFreeRate;
};

class OptionPricer {
    public:
    virtual ~OptionPricer() = default;
    virtual double price(Option& option, MarketData& marketData) = 0;
};

class BinomialOptionPricer final : public OptionPricer {
    public:

    BinomialOptionPricer(const double up, const double down, const int steps)
        : up_(up), down_(down), steps_(steps) {};

    double price(Option& option, MarketData& marketData) override {
        std::vector<double> values(steps_ + 1);
        for (int i { 0 }; i <= steps_; ++i) {
            const double spotPrice = marketData.spotPrice * std::pow(1 + up_, i) * std::pow(1 + down_, steps_ - i);
            values[i] = option.payoff(spotPrice);
        }

        const double discounting = 1.0 / (1 + marketData.riskFreeRate);
        const double riskNeutralProbability = (marketData.riskFreeRate - down_) / (up_ - down_);

        for (int i { steps_ - 1 }; i >= 0; --i) {
            for (int j { 0 }; j <= i ; ++j) {
                values[j] = discounting * (riskNeutralProbability * values[j+1] + (1 - riskNeutralProbability) * values[j]);
            }
        }

        return values[0];
    };

private:
    double up_;
    double down_;
    int steps_;
};

#endif //OPTION_PRICER_OPTIONPRICER_HPP