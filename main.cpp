#include <iostream>

#include "include/LatticeParam.hpp"
#include "include/Option.hpp"
#include "include/pricer/BinomialOptionPricer.hpp"

int main() {

    EuropeanCallOption europeanCall {50, 1.0};
    constexpr MarketData marketData {100.0, 0.05, 0.2};

    BinomialOptionPricer pricer { fromDiscrete(1.08, 0.93, 1.05, 5) };
    std::cout << pricer.price(europeanCall, marketData) << std::endl;

    BinomialOptionPricer crrPricer {CoxRoxRubinstein(marketData.volatility, marketData.riskFreeRate, europeanCall.getMaturity(), 100)};
    std::cout << crrPricer.price(europeanCall, marketData) << std::endl;

    return 0;
}
