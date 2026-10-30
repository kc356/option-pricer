#include <iostream>

#include "option-pricer/LatticeParam.hpp"
#include "option-pricer/option/AmericanOption.hpp"
#include "option-pricer/option/EuropeanOption.hpp"
#include "option-pricer/pricer/BinomialOptionPricer.hpp"
#include "option-pricer/pricer/BlackScholesAnalyticPricer.hpp"
#include "option-pricer/pricer/FiniteDifferencePricer.hpp"
#include "option-pricer/pricer/MonteCarloSimulationPricer.hpp"


int main() {

    EuropeanCallOption europeanCall {100, 1.0};
    EuropeanPutOption europeanPut {80, 1.0};
    constexpr MarketData marketData {100.0, 0.05, 0.2};
    //
    // // BinomialOptionPricer pricer { fromDiscrete(1.08, 0.9, 1.05, 3) };
    // // // std::cout << pricer.price(europeanCall, marketData) << std::endl;
    // // std::cout << pricer.price(europeanPut, marketData) << std::endl;
    //
    // BinomialOptionPricer crrPricer {CoxRoxRubinstein(marketData.volatility, marketData.riskFreeRate, europeanCall.getMaturity(), 100)};
    // std::cout << crrPricer.price(europeanCall, marketData).price << std::endl;
    // BlackScholesAnalyticPricer bsPricer {};
    // std::cout << bsPricer.price(europeanCall, marketData).price << std::endl;
    // FiniteDifferencePricer finiteDifferencePricer {2000, 200};
    // std::cout << finiteDifferencePricer.price(europeanCall, marketData).price << std::endl;
    // // std::cout << bsPricer.price(europeanPut, marketData) << std::endl;
    // MonteCarloSimulationPricer monteCarloPricer {1000000, 42};
    // std::cout << monteCarloPricer.price(europeanCall, marketData).price << std::endl;

    AmericanCallOption americanCall {100, 1.0};
    AmericanPutOption americanPut {80, 1.0};
    BinomialOptionPricer amCrrPricer {CoxRoxRubinstein(marketData.volatility, marketData.riskFreeRate, americanCall.getMaturity(), 100)};
    std::cout << amCrrPricer.price(americanCall, marketData).price << std::endl;
    std::cout << amCrrPricer.price(americanPut, marketData).price << std::endl;
    FiniteDifferencePricer finiteDifferencePricer {2000, 200};
    std::cout << finiteDifferencePricer.price(americanCall, marketData).price << std::endl;
    std::cout << finiteDifferencePricer.price(americanPut, marketData).price << std::endl;

    return 0;
}
