cmake_minimum_required(VERSION 4.0)
project(option_pricer)

set(CMAKE_CXX_STANDARD 20)

add_executable(option_pricer main.cpp
        include/option/Option.hpp
        include/pricer/OptionPricer.hpp
        include/LatticeParam.hpp
        src/pricer/BinomialOptionPricer.cpp
        include/pricer/BinomialOptionPricer.hpp
        src/pricer/BlackScholesAnalyticPricer.cpp
        include/pricer/BlackScholesAnalyticPricer.hpp
        src/pricer/FiniteDifferencePricer.cpp
        include/pricer/FiniteDifferencePricer.hpp
        src/pricer/MonteCarloSimulationPricer.cpp
        include/pricer/MonteCarloSimulationPricer.hpp
        src/option/EuropeanOption.cpp
        include/option/EuropeanOption.hpp)
