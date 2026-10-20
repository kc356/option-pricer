//
// Created by Kin on 10/19/2026.
//

#ifndef OPTION_PRICER_OPTION_HPP
#define OPTION_PRICER_OPTION_HPP
#include <algorithm>

// Price = Engine( Instrument, MarketData )

class Option {
public:
    virtual double payoff(double spotPrice) = 0;

    explicit Option(const double strikePrice)
        : strikePrice_(strikePrice) {};

    virtual ~Option() = default;
protected:
    double strikePrice_ {};

};

class EuropeanCallOption final : public Option {
public:
    double payoff(const double spotPrice) override { return std::max(spotPrice - strikePrice_, 0.0); };

    using Option::Option;
};

class EuropeanPutOption final : public Option{
public:
    double payoff(const double spotPrice) override { return std::max(strikePrice_ - spotPrice, 0.0); };

    using Option::Option;
};


#endif //OPTION_PRICER_OPTION_HPP