#include <iostream>

#include "include/BlackScholesAnalyticPricer.hpp"
#include "include/LatticeParam.hpp"
#include "include/Option.hpp"
#include "include/pricer/BinomialOptionPricer.hpp"

int main() {

    EuropeanCallOption europeanCall {50, 1.0};
    EuropeanPutOption europeanPut {80, 1.0};
    constexpr MarketData marketData {100.0, 0.05, 0.2};

    BinomialOptionPricer pricer { fromDiscrete(1.08, 0.9, 1.05, 3) };
    // std::cout << pricer.price(europeanCall, marketData) << std::endl;
    std::cout << pricer.price(europeanPut, marketData) << std::endl;

    // BinomialOptionPricer crrPricer {CoxRoxRubinstein(marketData.volatility, marketData.riskFreeRate, europeanCall.getMaturity(), 100)};
    // std::cout << crrPricer.price(europeanCall, marketData) << std::endl;
    //
    // BlackScholesAnalyticPricer bsPricer {};
    // std::cout << bsPricer.price(europeanCall, marketData) << std::endl;
    // std::cout << bsPricer.price(europeanPut, marketData) << std::endl;

    return 0;
}
