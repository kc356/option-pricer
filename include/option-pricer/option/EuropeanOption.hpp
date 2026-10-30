//
// Created by Kin on 10/25/2026.
//

#ifndef OPTION_PRICER_EUROPEANOPTION_HPP
#define OPTION_PRICER_EUROPEANOPTION_HPP
#include "Option.hpp"


class EuropeanCallOption final : public Option {
public:
    double payoff(double spotPrice) override;

    EuropeanCallOption(const double strikePrice, const double expiry)
        : Option {strikePrice, expiry, OptionType::Call}{};

    bool canExercise(double currentTime) const override;
};

class EuropeanPutOption final : public Option{
public:
    double payoff(double spotPrice) override;

    EuropeanPutOption(const double strikePrice, const double expiry)
        : Option {strikePrice, expiry, OptionType::Put}{};

    bool canExercise(double currentTime) const override;
};



#endif //OPTION_PRICER_EUROPEANOPTION_HPP