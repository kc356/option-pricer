#include <iostream>

#include "include/pricer/BlackScholesAnalyticPricer.hpp"
#include "include/LatticeParam.hpp"
#include "include/option/EuropeanOption.hpp"
#include "include/option/Option.hpp"
#include "include/pricer/BinomialOptionPricer.hpp"
#include "include/pricer/FiniteDifferencePricer.hpp"
#include "include/pricer/MonteCarloSimulationPricer.hpp"

int main() {

    EuropeanCallOption europeanCall {100, 1.0};
    EuropeanPutOption europeanPut {80, 1.0};
    constexpr MarketData marketData {100.0, 0.05, 0.2};

    // BinomialOptionPricer pricer { fromDiscrete(1.08, 0.9, 1.05, 3) };
    // // std::cout << pricer.price(europeanCall, marketData) << std::endl;
    // std::cout << pricer.price(europeanPut, marketData) << std::endl;

    BinomialOptionPricer crrPricer {CoxRoxRubinstein(marketData.volatility, marketData.riskFreeRate, europeanCall.getMaturity(), 100)};
    std::cout << crrPricer.price(europeanCall, marketData).price << std::endl;
    BlackScholesAnalyticPricer bsPricer {};
    std::cout << bsPricer.price(europeanCall, marketData).price << std::endl;
    FiniteDifferencePricer finiteDifferencePricer {2000, 200};
    std::cout << finiteDifferencePricer.price(europeanCall, marketData).price << std::endl;
    // std::cout << bsPricer.price(europeanPut, marketData) << std::endl;
    MonteCarloSimulationPricer monteCarloPricer {1000000, 42};
    std::cout << monteCarloPricer.price(europeanCall, marketData).price << std::endl;

    return 0;
}
