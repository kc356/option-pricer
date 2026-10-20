#include <iostream>

#include "include/Option.hpp"

int main() {

    EuropeanCallOption e {100, 50};

    std::cout << e.payoff();

    return 0;
}
