//
// Created by Kin on 10/19/2026.
//

#ifndef OPTION_PRICER_OPTION_HPP
#define OPTION_PRICER_OPTION_HPP


// Price = Engine( Instrument, MarketData )

class Option {
public:
    virtual double payoff(double spotPrice) = 0;

    explicit Option(const double strikePrice, const double expiry)
        : strikePrice_(strikePrice), expiry_(expiry) {};

    virtual ~Option() = default;

    double strike_price() const {
        return strikePrice_;
    }

    double expiry() const {
        return expiry_;
    }

protected:
    double strikePrice_ {};
    double expiry_ {};

};

class EuropeanCallOption final : public Option {
public:
    double payoff(double spotPrice) override;

    using Option::Option;
};

class EuropeanPutOption final : public Option{
public:
    double payoff(double spotPrice) override;

    using Option::Option;
};

#endif //OPTION_PRICER_OPTION_HPP