//
// Created by Kin on 10/25/2026.
//

#ifndef OPTION_PRICER_AMERICANOPTION_HPP
#define OPTION_PRICER_AMERICANOPTION_HPP
#include "Option.hpp"


class AmericanCallOption final : public Option{
public:
    double payoff(double spotPrice) override;

    AmericanCallOption(const double strikePrice, const double expiry)
        : Option {strikePrice, expiry, OptionType::Call}{};

    bool canExercise(double currentTime) const override;
};

class AmericanPutOption final : public Option{
public:
    double payoff(double spotPrice) override;

    AmericanPutOption(const double strikePrice, const double expiry)
        : Option {strikePrice, expiry, OptionType::Call}{};

    bool canExercise(double currentTime) const override;
};


#endif //OPTION_PRICER_AMERICANOPTION_HPP