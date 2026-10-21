//
// Created by Kin on 10/19/2026.
//

#ifndef OPTION_PRICER_OPTION_HPP
#define OPTION_PRICER_OPTION_HPP

class Option {
public:
    virtual double payoff(double spotPrice) = 0;

    explicit Option(const double strikePrice, const double expiry)
        : strikePrice_(strikePrice), maturity_(expiry) {};

    virtual ~Option() = default;

    double getStrikePrice() const {
        return strikePrice_;
    }

    double getMaturity() const {
        return maturity_;
    }

protected:
    double strikePrice_ {};
    double maturity_ {};

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