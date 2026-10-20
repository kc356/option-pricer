#include <iostream>

#include "include/Option.hpp"
#include "include/OptionPricer.hpp"

int main() {

    EuropeanCallOption europeanCall {50};

    BinomialOptionPricer pricer {0.08, -0.07, 3};

    MarketData marketData {100.0, 0.05};

    std::cout << pricer.price(europeanCall, marketData);

    // BinomialOptionPricer::printBinaryTree(pricer.spotPriceTreeRoot_);

    return 0;
}
