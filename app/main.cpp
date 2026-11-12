#include <iomanip>
#include <iostream>
#include <string>

#include "option-pricer/LatticeParam.hpp"
#include "option-pricer/option/AmericanOption.hpp"
#include "option-pricer/option/EuropeanOption.hpp"
#include "option-pricer/pricer/BinomialOptionPricer.hpp"
#include "option-pricer/pricer/BlackScholesAnalyticPricer.hpp"
#include "option-pricer/pricer/FiniteDifferencePricer.hpp"
#include "option-pricer/pricer/MonteCarloSimulationPricer.hpp"

namespace {

void printRow(const std::string& label, const PricerResult& result) {
    std::cout << "  " << std::left << std::setw(16) << label << std::right << std::setw(10)
              << result.price;
    if (result.standardError) {
        std::cout << "  (s.e. " << *result.standardError << ")";
    }
    std::cout << '\n';
}

} // namespace

int main() {
    constexpr double strike = 100.0;
    constexpr double expiry = 1.0;
    constexpr MarketData marketData {100.0, 0.05, 0.2, 0.02};

    EuropeanCallOption europeanCall {strike, expiry};
    EuropeanPutOption europeanPut {strike, expiry};
    AmericanCallOption americanCall {strike, expiry};
    AmericanPutOption americanPut {strike, expiry};

    BlackScholesAnalyticPricer blackScholes;
    BinomialOptionPricer binomial {CoxRossRubinstein(marketData.volatility, marketData.riskFreeRate,
                                                    expiry, 1000, marketData.dividendYield)};
    FiniteDifferencePricer finiteDifference {500, 400};
    MonteCarloSimulationPricer monteCarlo {1000000, 42};

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "S = " << marketData.spotPrice << ", K = " << strike << ", T = " << expiry
              << ", r = " << marketData.riskFreeRate << ", sigma = " << marketData.volatility
              << ", q = " << marketData.dividendYield << "\n\n";

    for (Option* option :
         {static_cast<Option*>(&europeanCall), static_cast<Option*>(&europeanPut)}) {
        std::cout << "European " << (option->getOptionType() == OptionType::Call ? "call" : "put")
                  << '\n';
        printRow("Black-Scholes", blackScholes.price(*option, marketData));
        printRow("Binomial (CRR)", binomial.price(*option, marketData));
        printRow("Crank-Nicolson", finiteDifference.price(*option, marketData));
        printRow("Monte Carlo", monteCarlo.price(*option, marketData));
        std::cout << '\n';
    }

    for (Option* option :
         {static_cast<Option*>(&americanCall), static_cast<Option*>(&americanPut)}) {
        std::cout << "American " << (option->getOptionType() == OptionType::Call ? "call" : "put")
                  << '\n';
        printRow("Binomial (CRR)", binomial.price(*option, marketData));
        printRow("Crank-Nicolson", finiteDifference.price(*option, marketData));
        std::cout << '\n';
    }

    return 0;
}
