//
// Created by Kin on 11/12/2026.
//

#include <gtest/gtest.h>

#include "option-pricer/option/AmericanOption.hpp"
#include "option-pricer/option/EuropeanOption.hpp"
#include "option-pricer/pricer/BinomialOptionPricer.hpp"
#include "option-pricer/pricer/BlackScholesAnalyticPricer.hpp"
#include "option-pricer/pricer/FiniteDifferencePricer.hpp"

namespace {

constexpr double kTolerance = 5e-3;

class FiniteDifferencePricerTest : public ::testing::TestWithParam<MarketData> {
protected:
    FiniteDifferencePricer pricer {500, 400};
    BlackScholesAnalyticPricer bs;
};

TEST_P(FiniteDifferencePricerTest, EuropeanCallMatchesBlackScholes) {
    EuropeanCallOption call {100.0, 1.0};
    EXPECT_NEAR(pricer.price(call, GetParam()).price, bs.price(call, GetParam()).price, kTolerance);
}

TEST_P(FiniteDifferencePricerTest, EuropeanPutMatchesBlackScholes) {
    EuropeanPutOption put {100.0, 1.0};
    EXPECT_NEAR(pricer.price(put, GetParam()).price, bs.price(put, GetParam()).price, kTolerance);
}

TEST_P(FiniteDifferencePricerTest, AmericanMatchesBinomial) {
    const MarketData& marketData = GetParam();
    BinomialOptionPricer binomial {CoxRossRubinstein(marketData.volatility, marketData.riskFreeRate,
                                                    1.0, 2000, marketData.dividendYield)};
    AmericanCallOption call {100.0, 1.0};
    AmericanPutOption put {100.0, 1.0};
    EXPECT_NEAR(pricer.price(call, marketData).price, binomial.price(call, marketData).price,
                kTolerance);
    EXPECT_NEAR(pricer.price(put, marketData).price, binomial.price(put, marketData).price,
                kTolerance);
}

TEST_P(FiniteDifferencePricerTest, AmericanIsWorthAtLeastEuropean) {
    AmericanPutOption american {100.0, 1.0};
    EuropeanPutOption european {100.0, 1.0};
    EXPECT_GE(pricer.price(american, GetParam()).price,
              pricer.price(european, GetParam()).price - 1e-12);
}

INSTANTIATE_TEST_SUITE_P(MarketScenarios, FiniteDifferencePricerTest,
                         ::testing::Values(MarketData {100.0, 0.05, 0.2},
                                           MarketData {90.0, 0.03, 0.3, 0.02},
                                           MarketData {110.0, 0.05, 0.2, 0.08}));

} // namespace
