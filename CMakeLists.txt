cmake_minimum_required(VERSION 4.0)
project(option_pricer)

set(CMAKE_CXX_STANDARD 20)

add_executable(option_pricer main.cpp
        src/Option.cpp
        include/Option.hpp
        include/pricer/OptionPricer.hpp
        include/LatticeParam.hpp
        src/pricer/BinomialOptionPricer.cpp
        include/pricer/BinomialOptionPricer.hpp)
