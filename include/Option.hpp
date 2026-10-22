//
// Created by Kin on 10/19/2026.
//

#ifndef OPTION_PRICER_OPTION_HPP
#define OPTION_PRICER_OPTION_HPP

enum class OptionType {
    Call,
    Put
};

class Option {
public:
    virtual double payoff(double spotPrice) = 0;

    Option(const double strikePrice, const double expiry, const OptionType optionType)
        : strikePrice_(strikePrice), maturity_(expiry), optionType_(optionType) {};

    virtual ~Option() = default;

    double getStrikePrice() const {
        return strikePrice_;
    }

    double getMaturity() const {
        return maturity_;
    }

    OptionType getOptionType() const {
        return optionType_;
    }

protected:
    double strikePrice_ {};
    double maturity_ {};
    OptionType optionType_ {};
};

class EuropeanCallOption final : public Option {
public:
    double payoff(double spotPrice) override;

    EuropeanCallOption(const double strikePrice, const double expiry)
        : Option {strikePrice, expiry, OptionType::Call}{};
};

class EuropeanPutOption final : public Option{
public:
    double payoff(double spotPrice) override;

    EuropeanPutOption(const double strikePrice, const double expiry)
        : Option {strikePrice, expiry, OptionType::Put}{};
};

#endif //OPTION_PRICER_OPTION_HPP