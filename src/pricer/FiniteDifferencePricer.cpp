//
// Created by Kin on 10/22/2026.
//

#include "option-pricer/pricer/FiniteDifferencePricer.hpp"

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

#include "option-pricer/pricer/OptionPricer.hpp"

namespace {

constexpr int kRannacherSteps = 2;
constexpr double kCrankNicolsonTheta = 0.5;

constexpr double kSorOmega = 1.2;
constexpr double kSorTolerance = 1e-10;
constexpr int kSorMaxIterations = 10000;

struct Tridiagonal {
    std::vector<double> lower, diag, upper;
};

void solveTridiagonal(const Tridiagonal& m, std::vector<double>& rhs, const int n) {
    std::vector<double> c(n);
    c[1] = m.upper[1] / m.diag[1];
    rhs[1] /= m.diag[1];
    for (int i {2}; i < n; ++i) {
        const double denom = m.diag[i] - m.lower[i] * c[i - 1];
        c[i] = m.upper[i] / denom;
        rhs[i] = (rhs[i] - m.lower[i] * rhs[i - 1]) / denom;
    }
    for (int i {n - 2}; i >= 1; --i) {
        rhs[i] -= c[i] * rhs[i + 1];
    }
}

void solveProjectedSor(const Tridiagonal& m, const std::vector<double>& rhs,
                       const std::vector<double>& floor, std::vector<double>& x, const int n) {
    for (int iter {0}; iter < kSorMaxIterations; ++iter) {
        double error {0.0};
        for (int i {1}; i < n; ++i) {
            const double gaussSeidel =
                (rhs[i] - m.lower[i] * x[i - 1] - m.upper[i] * x[i + 1]) / m.diag[i];
            const double updated = std::max(floor[i], x[i] + kSorOmega * (gaussSeidel - x[i]));
            error += (updated - x[i]) * (updated - x[i]);
            x[i] = updated;
        }
        if (error < kSorTolerance * kSorTolerance) {
            return;
        }
    }
}

} // namespace

PricerResult FiniteDifferencePricer::price(Option& option, const MarketData& marketData) {
    const double T = option.getMaturity();
    const double K = option.getStrikePrice();
    const double r = marketData.riskFreeRate;
    const double q = marketData.dividendYield;
    const double sigma2 = marketData.volatility * marketData.volatility;

    const int n = priceSteps_;
    const double dt = T / timeSteps_;
    const double sMax = 4.0 * std::max(marketData.spotPrice, K);
    const double dS = sMax / n;

    std::vector<double> payoffs(n + 1);
    for (int i {0}; i <= n; ++i) {
        payoffs[i] = option.payoff(i * dS);
    }

    // dV/dtau = 0.5 sigma^2 S^2 V_SS + (r - q) S V_S - r V.
    std::vector<double> alpha(n), beta(n), gamma(n);
    for (int i {1}; i < n; ++i) {
        alpha[i] = 0.5 * (sigma2 * i * i - (r - q) * i);
        beta[i] = -(sigma2 * i * i + r);
        gamma[i] = 0.5 * (sigma2 * i * i + (r - q) * i);
    }

    const auto boundaryValues = [&](const double tau, const bool exercisable) {
        double lower {0.0}, upper {0.0};
        if (option.getOptionType() == OptionType::Put) {
            lower = K * std::exp(-r * tau);
        } else {
            upper = sMax * std::exp(-q * tau) - K * std::exp(-r * tau);
        }
        if (exercisable) {
            lower = std::max(lower, payoffs[0]);
            upper = std::max(upper, payoffs[n]);
        }
        return std::pair {lower, upper};
    };

    std::vector<double> values = payoffs, rhs(n + 1);
    Tridiagonal lhs {std::vector<double>(n), std::vector<double>(n), std::vector<double>(n)};

    for (int step {0}; step < timeSteps_; ++step) {
        const double theta = step < kRannacherSteps ? 1.0 : kCrankNicolsonTheta;
        const double tau = (step + 1) * dt;
        const bool exercisable = option.canExercise(T - tau);

        for (int i {1}; i < n; ++i) {
            rhs[i] = values[i] + (1.0 - theta) * dt *
                                     (alpha[i] * values[i - 1] + beta[i] * values[i] +
                                      gamma[i] * values[i + 1]);
            lhs.lower[i] = -theta * dt * alpha[i];
            lhs.diag[i] = 1.0 - theta * dt * beta[i];
            lhs.upper[i] = -theta * dt * gamma[i];
        }

        const auto [lower, upper] = boundaryValues(tau, exercisable);

        if (exercisable) {
            values[0] = lower;
            values[n] = upper;
            for (int i {1}; i < n; ++i) {
                values[i] = std::max(values[i], payoffs[i]);
            }
            solveProjectedSor(lhs, rhs, payoffs, values, n);
        } else {
            rhs[1] -= lhs.lower[1] * lower;
            rhs[n - 1] -= lhs.upper[n - 1] * upper;
            solveTridiagonal(lhs, rhs, n);
            std::copy(rhs.begin() + 1, rhs.begin() + n, values.begin() + 1);
            values[0] = lower;
            values[n] = upper;
        }
    }

    const double x = marketData.spotPrice / dS;
    const int i = static_cast<int>(x);
    const double w = x - i;
    return {(1.0 - w) * values[i] + w * values[i + 1]};
}
