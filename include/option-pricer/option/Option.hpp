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
    virtual double payoff(double spotPrice) const = 0;

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

    virtual bool canExercise(double currentTime) const = 0;

protected:
    double strikePrice_ {};
    double maturity_ {};
    OptionType optionType_ {};
};


#endif //OPTION_PRICER_OPTION_HPP