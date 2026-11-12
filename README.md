# option-pricer

A small C++20 library that prices vanilla European and American options four different ways, then
checks them against each other.

It started as a weekend experiment in October 2026 and grew into a project about design trade-offs:
how should an option, a pricing engine and market data relate to each other? The day-by-day story,
including the rewrites and mistakes, is in [devlog.md](devlog.md).

```
S = 100, K = 100, T = 1, r = 0.05, sigma = 0.2, q = 0.02

European call                       American put
  Black-Scholes       9.2270          Binomial (CRR)      6.6598
  Binomial (CRR)      9.2251          Crank-Nicolson      6.6577
  Crank-Nicolson      9.2246
  Monte Carlo         9.2473  (s.e. 0.0146)
```

## Design

The guiding idea from day one:

```
Price = Engine( Instrument, MarketData )
```

- **Instrument:** `Option` is an abstract class with a strike, a maturity and a call/put type. It has two
  virtual functions: `payoff(S)` and `canExercise(t)`. European options can only be exercised at
  maturity. American options can be exercised at any time. Because exercise rules belong to the
  instrument, any pricer that steps backwards in time handles both styles with the same code.
- **MarketData:** spot $S$, risk-free rate $r$, volatility $\sigma$ and an optional continuous dividend
  yield $q$. The spot price lives here rather than on the option, because the contract doesn't
  change when the market does.
- **Engine:** `OptionPricer::price(Option&, const MarketData&)` returns a `PricerResult`: a price,
  plus a standard error for pricers that have one. Settings specific to each method (tree steps,
  grid size, path count, seed) are passed to the pricer's constructor, so `price()` has the same
  signature for every pricer.

## Pricers

| Pricer | Method | European | American | Dividends |
|---|---|:-:|:-:|:-:|
| `BlackScholesAnalyticPricer` | Closed form (Black–Scholes–Merton) | ✓ | ✗ | ✓ |
| `BinomialOptionPricer` | Recombining binomial tree, backward induction | ✓ | ✓ | ✓ |
| `FiniteDifferencePricer` | Crank–Nicolson finite differences on the Black–Scholes PDE | ✓ | ✓ | ✓ |
| `MonteCarloSimulationPricer` | Simulates terminal prices under GBM | ✓ | ✗ | ✓ |

Pricers that can't handle American options refuse them at compile time: they declare
`price(AmericanCallOption&, …)` and `price(AmericanPutOption&, …)` as `= delete`. This only works
when the compiler can see the concrete option type. A call through an `Option&` still compiles.

### Black–Scholes

The textbook formula with a continuous dividend yield:

```math
\begin{aligned}
d_1 &= \frac{\ln(S/K) + \left(r - q + \tfrac{1}{2}\sigma^2\right)T}{\sigma\sqrt{T}}, \qquad d_2 = d_1 - \sigma\sqrt{T} \\
C &= S e^{-qT} N(d_1) - K e^{-rT} N(d_2) \\
P &= K e^{-rT} N(-d_2) - S e^{-qT} N(-d_1)
\end{aligned}
```

The standard normal CDF $N(\cdot)$ is computed with `std::erfc`, using $N(x) = \tfrac{1}{2}\operatorname{erfc}\left(-x/\sqrt{2}\right)$. American options have no closed-form price, so this
pricer only handles European ones.

### Binomial tree

`LatticeParam` describes the tree. There are two ways to build one:

- `CoxRossRubinstein(sigma, r, T, steps, q)`: with $\Delta t = T / \text{steps}$,

  ```math
  u = e^{\sigma\sqrt{\Delta t}}, \qquad d = \frac{1}{u}, \qquad p = \frac{e^{(r - q)\Delta t} - d}{u - d}
  ```

  where $p$ is the risk-neutral probability of an up move.
- `fromDiscrete(u, d, R, steps)`: the classroom one-period model with a gross rate $R$, so
  $p = (R - d)/(u - d)$.

The first version built an actual tree of heap-allocated nodes. The tree didn't recombine, so the
node count grew exponentially, and it also leaked memory. The current version keeps a single
`std::vector` of `steps + 1` terminal values and works backwards in place. When the option can be
exercised early, each node takes $\max(\text{continuation}, \text{payoff})$.

### Crank–Nicolson finite differences

This pricer solves the Black–Scholes PDE in time to maturity $\tau = T - t$:

```math
\frac{\partial V}{\partial \tau} = \frac{1}{2}\sigma^2 S^2 \frac{\partial^2 V}{\partial S^2} + (r - q) S \frac{\partial V}{\partial S} - rV
```

It works on a uniform grid $S_i = i\,\Delta S$ for $i = 0, \dots, N$, with
$S_{\max} = 4\max(S, K)$. Writing the discretised right-hand side as an operator $L$, each time
step solves

```math
\left(I - \theta\,\Delta t\,L\right) V^{n+1} = \left(I + (1 - \theta)\,\Delta t\,L\right) V^{n}
```

where row $i$ of $L$ is

```math
(LV)_i = \underbrace{\tfrac{1}{2}\left(\sigma^2 i^2 - (r - q)\,i\right)}_{\alpha_i} V_{i-1}
       \underbrace{- \left(\sigma^2 i^2 + r\right)}_{\beta_i} V_i
       + \underbrace{\tfrac{1}{2}\left(\sigma^2 i^2 + (r - q)\,i\right)}_{\gamma_i} V_{i+1}
```

The scheme is set up as follows:

- **Crank–Nicolson ($\theta = \tfrac{1}{2}$):** accurate to second order in both time and price,
  $O(\Delta t^2 + \Delta S^2)$, and stable for any time step. The original explicit scheme blew up
  when $\Delta t$ was too large compared with $\Delta S$.
- **Rannacher start-up:** the first two time steps are fully implicit ($\theta = 1$). This damps the
  oscillations Crank–Nicolson produces around the kink in the payoff at the strike.
- **European options:** each step is a tridiagonal system, solved directly with the Thomas
  algorithm in $O(N)$.
- **American options:** each step is solved with projected SOR (PSOR), which keeps every value at
  or above the exercise payoff during the solve itself: $V_i \ge \text{payoff}(S_i)$.
- **Boundaries:** fixed values from the asymptotic behaviour at both ends ($K e^{-r\tau}$ for a put at
  $S = 0$, $S_{\max} e^{-q\tau} - K e^{-r\tau}$ for a call at $S_{\max}$), floored at the payoff for American
  options.

The price at today's spot is interpolated linearly between the two nearest grid points.

### Monte Carlo

This pricer samples terminal prices directly from geometric Brownian motion,

```math
S_T = S \exp\left[\left(r - q - \tfrac{1}{2}\sigma^2\right)T + \sigma\sqrt{T}\,Z\right], \qquad Z \sim \mathcal{N}(0, 1)
```

With $M$ paths it reports the discounted mean payoff and its standard error,

```math
\hat{V} = e^{-rT}\,\bar{X}, \qquad \text{s.e.} = \sqrt{\frac{\overline{X^2} - \bar{X}^2}{M}}
```

where $X$ is the payoff on a single path and the bars denote averages over the $M$ paths. Runs are reproducible:
the `std::mt19937_64` engine is seeded on every call. It only samples the price at maturity, so it
can't price American options. That would need a method such as Longstaff–Schwartz.

## Usage

```cpp
#include "option-pricer/option/AmericanOption.hpp"
#include "option-pricer/pricer/FiniteDifferencePricer.hpp"

AmericanPutOption put {100.0, 1.0};                  // strike, maturity (years)
constexpr MarketData market {100.0, 0.05, 0.2, 0.02}; // S, r, sigma, q (q is optional)

FiniteDifferencePricer pricer {500, 400};            // time steps, price steps
const PricerResult result = pricer.price(put, market);
// result.price ≈ 6.658
```

## Building

Requirements: CMake ≥ 3.24 and a C++20 compiler. CMake downloads GoogleTest and Google Benchmark
automatically with `FetchContent`.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

./build/option_pricer_demo                    # comparison table above
./build/tests/option_pricer_tests             # unit tests (or: ctest --test-dir build)
./build/benchmarks/option_pricer_bench        # benchmarks
```

| Target | What it is |
|---|---|
| `option_pricer` | The library (`include/`, `src/`) |
| `option_pricer_demo` | `app/main.cpp`, compares every pricer on the same market |
| `option_pricer_tests` | GoogleTest suite, on by default (`OPTION_PRICER_BUILD_TESTS`) |
| `option_pricer_bench` | Google Benchmark suite, on by default (`OPTION_PRICER_BUILD_BENCHMARKS`) |

The library compiles warning-free with `-Wall -Wextra -Wpedantic`. Formatting is set by
`.clang-format`.

## Tests

The suite checks each pricer against independent reference values, against the other pricers, and
against relationships that must always hold:

- **Black–Scholes:** matches reference prices computed independently, with and without dividends.
  Satisfies put–call parity, $C - P = S e^{-qT} - K e^{-rT}$. Dividends lower the call price and
  raise the put price.
- **Binomial:** a one-step tree matches a hand calculation, and the European price converges to
  Black–Scholes. The American put matches the well-known value of 6.0904 ($S = K = 100$, $r = 5\%$,
  $\sigma = 20\%$, $T = 1$). An American call without dividends is worth exactly the same as the
  European call. With dividends, it's worth more.
- **Crank–Nicolson:** tested on three market scenarios. European prices are within 5e-3 of
  Black–Scholes, American prices are within 5e-3 of a 2000-step binomial tree, and an American
  option is never worth less than its European counterpart.
- **Monte Carlo:** within 4 standard errors of Black–Scholes, and the same seed gives the same
  price.

## Benchmarks

Measured on an Apple Silicon Mac in a Release build, with $S = K = 100$, $r = 5\%$, $\sigma = 20\%$,
$q = 2\%$, $T = 1$. Each Big-O column is fitted by Google Benchmark across the sizes shown.

**Black–Scholes:** 21 ns per price.

**Binomial tree** (time per price):

| Steps | European put | American put |
|---:|---:|---:|
| 128 | 3.6 µs | 137 µs |
| 256 | 9.6 µs | 539 µs |
| 512 | 29 µs | 2.1 ms |
| 1024 | 100 µs | 8.6 ms |
| 2048 | 355 µs | 34 ms |
| 4096 | 1.35 ms | 137 ms |
| *Fitted* | $O(N^2)$ | $O(N^2)$ |

**Crank–Nicolson** ($N$ time steps $\times$ $N$ price steps):

| N | European put | American put |
|---:|---:|---:|
| 100 | 0.063 ms | 0.91 ms |
| 256 | 0.41 ms | 5.5 ms |
| 512 | 1.6 ms | 26 ms |
| 1024 | 6.6 ms | 173 ms |
| 1600 | 16.6 ms | 596 ms |
| *Fitted* | $O(N^2)$ | $O(N^3)$ |

**Monte Carlo** (European put):

| Paths | Time | Throughput |
|---:|---:|---:|
| 10,000 | 0.26 ms | 38 M paths/s |
| 100,000 | 3.1 ms | 32 M paths/s |
| 1,000,000 | 31 ms | 32 M paths/s |

**Raw output**:
```bash
/Users/kc356/CLionProjects/option-pricer/cmake-build-release/benchmarks/option_pricer_bench
Unable to determine clock rate from sysctl: hw.cpufrequency: No such file or directory
This does not affect benchmark measurements, only the metadata output.
***WARNING*** Failed to set thread affinity. Estimated CPU frequency may be incorrect.
2026-11-12T20:01:39+08:00
Running /Users/kc356/CLionProjects/option-pricer/cmake-build-release/benchmarks/option_pricer_bench
Run on (12 X 24 MHz CPU s)
CPU Caches:
  L1 Data 64 KiB
  L1 Instruction 128 KiB
  L2 Unified 4096 KiB (x12)
Load Average: 3.94, 4.11, 3.57
-----------------------------------------------------------------------------------
Benchmark                                         Time             CPU   Iterations
-----------------------------------------------------------------------------------
BM_BlackScholes                                22.0 ns         21.9 ns     31016408
BM_Binomial<EuropeanPutOption>/128             3787 ns         3732 ns       181805
BM_Binomial<EuropeanPutOption>/256             9553 ns         9544 ns        73581
BM_Binomial<EuropeanPutOption>/512            29588 ns        29575 ns        23050
BM_Binomial<EuropeanPutOption>/1024           99629 ns        99593 ns         7017
BM_Binomial<EuropeanPutOption>/2048          358900 ns       358611 ns         1955
BM_Binomial<EuropeanPutOption>/4096         1359406 ns      1358675 ns          511
BM_Binomial<EuropeanPutOption>_BigO            0.08 N^2        0.08 N^2  
BM_Binomial<EuropeanPutOption>_RMS                3 %             3 %    
BM_Binomial<AmericanPutOption>/128           136662 ns       136563 ns         5159
BM_Binomial<AmericanPutOption>/256           538482 ns       538291 ns         1308
BM_Binomial<AmericanPutOption>/512          2150595 ns      2149422 ns          327
BM_Binomial<AmericanPutOption>/1024         8527407 ns      8523427 ns           82
BM_Binomial<AmericanPutOption>/2048        34335016 ns     34312619 ns           21
BM_Binomial<AmericanPutOption>/4096       136288467 ns    136241000 ns            5
BM_Binomial<AmericanPutOption>_BigO            8.13 N^2        8.12 N^2  
BM_Binomial<AmericanPutOption>_RMS                0 %             0 %    
BM_CrankNicolson<EuropeanPutOption>/100       0.063 ms        0.063 ms        11231
BM_CrankNicolson<EuropeanPutOption>/128       0.104 ms        0.104 ms         6737
BM_CrankNicolson<EuropeanPutOption>/256       0.409 ms        0.409 ms         1712
BM_CrankNicolson<EuropeanPutOption>/512        1.64 ms         1.64 ms          427
BM_CrankNicolson<EuropeanPutOption>/1024       6.61 ms         6.60 ms          107
BM_CrankNicolson<EuropeanPutOption>/1600       16.5 ms         16.5 ms           42
BM_CrankNicolson<EuropeanPutOption>_BigO       6.43 N^2        6.42 N^2  
BM_CrankNicolson<EuropeanPutOption>_RMS           1 %             1 %    
BM_CrankNicolson<AmericanPutOption>/100       0.916 ms        0.916 ms          756
BM_CrankNicolson<AmericanPutOption>/128        1.47 ms         1.47 ms          479
BM_CrankNicolson<AmericanPutOption>/256        5.55 ms         5.54 ms          127
BM_CrankNicolson<AmericanPutOption>/512        27.6 ms         27.2 ms           26
BM_CrankNicolson<AmericanPutOption>/1024        176 ms          175 ms            4
BM_CrankNicolson<AmericanPutOption>/1600        630 ms          621 ms            1
BM_CrankNicolson<AmericanPutOption>_BigO       0.15 N^3        0.15 N^3  
BM_CrankNicolson<AmericanPutOption>_RMS           4 %             4 %    
BM_MonteCarlo/10000                           0.269 ms        0.269 ms         2577 items_per_second=37.2081M/s
BM_MonteCarlo/100000                           3.15 ms         3.15 ms          223 items_per_second=31.757M/s
BM_MonteCarlo/1000000                          31.2 ms         31.2 ms           23 items_per_second=32.0918M/s
BM_MonteCarlo_BigO                            31.19 N         31.16 N    
BM_MonteCarlo_RMS                                 0 %             0 %    

Process finished with exit code 0
```
To reproduce the results, or to run only some of them:

```sh
./build/benchmarks/option_pricer_bench --benchmark_filter=CrankNicolson \
    --benchmark_repetitions=5 --benchmark_report_aggregates_only=true
```

## Project layout

```
include/option-pricer/
  LatticeParam.hpp           MarketData, LatticeParam, CRR / discrete lattice builders
  option/                    Option base class, European and American calls/puts
  pricer/                    OptionPricer interface and the four pricers
src/                         Implementations
app/main.cpp                 Demo: every pricer on the same market
tests/                       GoogleTest suites, one per pricer
benchmarks/bench_pricers.cpp Google Benchmark suite
devlog.md                    Development diary
```

## Roadmap

From the plan in the devlog:

- [x] American options in the finite-difference pricer
- [x] Disable the analytic and Monte Carlo pricers for American options
- [x] Crank–Nicolson finite differences
- [x] Unit tests (GoogleTest)
- [x] Continuous dividend yield $q$
- [ ] Greeks: delta, gamma, vega, theta, rho
- [ ] Implied volatility solver (goodbye, Excel Solver)
- [x] Benchmarks
- [ ] Compare results with QuantLib
- [x] README

Also on the list:

- [ ] Fix the two performance problems found by the benchmarks (American binomial, PSOR scaling)
- [ ] Generalise the design to more instruments: Asian, digital and barrier options. This is
      where Monte Carlo and finite differences become essential.
