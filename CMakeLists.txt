cmake_minimum_required(VERSION 3.24)
project(option_pricer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(OPTION_PRICER_BUILD_TESTS      "Build unit tests"  ${PROJECT_IS_TOP_LEVEL})
option(OPTION_PRICER_BUILD_BENCHMARKS "Build benchmarks"  ${PROJECT_IS_TOP_LEVEL})

# ---- Library ----
add_library(option_pricer
        src/option/EuropeanOption.cpp
        src/option/AmericanOption.cpp
        src/pricer/BinomialOptionPricer.cpp
        src/pricer/BlackScholesAnalyticPricer.cpp
        src/pricer/FiniteDifferencePricer.cpp
        src/pricer/MonteCarloSimulationPricer.cpp
        tests/test_binomial.cpp)
add_library(option_pricer::option_pricer ALIAS option_pricer)

target_include_directories(option_pricer PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)
target_compile_features(option_pricer PUBLIC cxx_std_20)
target_compile_options(option_pricer PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>)

# ---- Demo executable ----
add_executable(option_pricer_demo app/main.cpp)
target_link_libraries(option_pricer_demo PRIVATE option_pricer)

# ---- Tests / benchmarks ----
if(OPTION_PRICER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(OPTION_PRICER_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()