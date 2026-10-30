include(FetchContent)

set(BENCHMARK_ENABLE_TESTING      OFF CACHE BOOL "" FORCE)  # don't build benchmark's own tests
set(BENCHMARK_ENABLE_GTEST_TESTS  OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL      OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        benchmark
        URL https://github.com/google/benchmark/archive/refs/tags/v1.9.4.zip)
FetchContent_MakeAvailable(benchmark)

add_executable(option_pricer_bench bench_pricers.cpp)
target_link_libraries(option_pricer_bench PRIVATE option_pricer benchmark::benchmark_main)