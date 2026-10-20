//
// Created by Kin on 10/19/2026.
//

#ifndef OPTION_PRICER_OPTION_HPP
#define OPTION_PRICER_OPTION_HPP
#include <algorithm>

// Price = Engine( Instrument, MarketData )

class Option {
public:
    virtual double payoff() = 0;

    Option(const double spotPrice, const double strikePrice) :
    spotPrice(spotPrice), strikePrice(strikePrice) {};

    virtual ~Option() = default;
protected:
    double spotPrice {};
    double strikePrice {};

};

class EuropeanCallOption : public Option {
public:
    double payoff() { return std::max(spotPrice - strikePrice, 0.0); };

    EuropeanCallOption(const double spotPrice, const double strikePrice) : Option {spotPrice, strikePrice} {};
};

class EuropeanPutOption : public Option{
public:
    double payoff() { return std::max(strikePrice - spotPrice, 0.0); };

    EuropeanPutOption(const double spotPrice, const double strikePrice) : Option {spotPrice, strikePrice} {};
};


#endif //OPTION_PRICER_OPTION_HPP