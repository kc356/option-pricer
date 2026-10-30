include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)   # needed on Windows/MSVC
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(option_pricer_tests
        test_black_scholes.cpp
        test_binomial.cpp)
target_link_libraries(option_pricer_tests PRIVATE option_pricer GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(option_pricer_tests)