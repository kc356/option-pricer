//
// Created by Kin on 10/30/2026.
//

#include <benchmark/benchmark.h>

#include "option-pricer/option/AmericanOption.hpp"
#include "option-pricer/option/EuropeanOption.hpp"
#include "option-pricer/pricer/BinomialOptionPricer.hpp"
#include "option-pricer/pricer/BlackScholesAnalyticPricer.hpp"
#include "option-pricer/pricer/FiniteDifferencePricer.hpp"
#include "option-pricer/pricer/MonteCarloSimulationPricer.hpp"

namespace {

constexpr MarketData kMarketData {100.0, 0.05, 0.2, 0.02};
constexpr double kStrike = 100.0;
constexpr double kExpiry = 1.0;

void BM_BlackScholes(benchmark::State& state) {
    EuropeanCallOption option {kStrike, kExpiry};
    BlackScholesAnalyticPricer pricer;
    for (auto _ : state) {
        benchmark::DoNotOptimize(pricer.price(option, kMarketData));
    }
}
BENCHMARK(BM_BlackScholes);

template <typename OptionT> void BM_Binomial(benchmark::State& state) {
    const int steps = static_cast<int>(state.range(0));
    OptionT option {kStrike, kExpiry};
    BinomialOptionPricer pricer {CoxRossRubinstein(kMarketData.volatility, kMarketData.riskFreeRate,
                                                   kExpiry, steps, kMarketData.dividendYield)};
    for (auto _ : state) {
        benchmark::DoNotOptimize(pricer.price(option, kMarketData));
    }
    state.SetComplexityN(steps);
}
BENCHMARK_TEMPLATE(BM_Binomial, EuropeanPutOption)
    ->RangeMultiplier(2)
    ->Range(128, 4096)
    ->Complexity(benchmark::oNSquared);
BENCHMARK_TEMPLATE(BM_Binomial, AmericanPutOption)
    ->RangeMultiplier(2)
    ->Range(128, 4096)
    ->Complexity(benchmark::oNSquared);

// Grid is N x N (time steps x price steps), so cost should scale as N^2 for European options.
template <typename OptionT> void BM_CrankNicolson(benchmark::State& state) {
    const int n = static_cast<int>(state.range(0));
    OptionT option {kStrike, kExpiry};
    FiniteDifferencePricer pricer {n, n};
    for (auto _ : state) {
        benchmark::DoNotOptimize(pricer.price(option, kMarketData));
    }
    state.SetComplexityN(n);
}
BENCHMARK_TEMPLATE(BM_CrankNicolson, EuropeanPutOption)
    ->RangeMultiplier(2)
    ->Range(100, 1600)
    ->Unit(benchmark::kMillisecond)
    ->Complexity(benchmark::oNSquared);
BENCHMARK_TEMPLATE(BM_CrankNicolson, AmericanPutOption)
    ->RangeMultiplier(2)
    ->Range(100, 1600)
    ->Unit(benchmark::kMillisecond)
    ->Complexity();

void BM_MonteCarlo(benchmark::State& state) {
    const long paths = static_cast<long>(state.range(0));
    EuropeanPutOption option {kStrike, kExpiry};
    MonteCarloSimulationPricer pricer {paths, 42};
    for (auto _ : state) {
        benchmark::DoNotOptimize(pricer.price(option, kMarketData));
    }
    state.SetItemsProcessed(state.iterations() * paths);
    state.SetComplexityN(paths);
}
BENCHMARK(BM_MonteCarlo)
    ->RangeMultiplier(10)
    ->Range(10'000, 1'000'000)
    ->Unit(benchmark::kMillisecond)
    ->Complexity(benchmark::oN);

} // namespace
