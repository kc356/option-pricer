//
// Created by Kin on 10/30/2026.
//

#include <gtest/gtest.h>

#include "option-pricer/option/AmericanOption.hpp"
#include "option-pricer/option/EuropeanOption.hpp"
#include "option-pricer/pricer/BinomialOptionPricer.hpp"
#include "option-pricer/pricer/BlackScholesAnalyticPricer.hpp"

namespace {

constexpr int kSteps = 1000;

BinomialOptionPricer crrPricer(const MarketData& marketData, const double expiry) {
    return BinomialOptionPricer {CoxRossRubinstein(marketData.volatility, marketData.riskFreeRate,
                                                  expiry, kSteps, marketData.dividendYield)};
}

TEST(BinomialOptionPricer, DiscreteOneStepMatchesHandCalculation) {
    // u = 1.2, d = 0.8, gross rate 1.05: q = 0.625, call payoff 20 in the up state only.
    EuropeanCallOption call {100.0, 1.0};
    BinomialOptionPricer pricer {fromDiscrete(1.2, 0.8, 1.05, 1)};
    EXPECT_NEAR(pricer.price(call, {100.0, 1.05, 0.0}).price, 0.625 * 20.0 / 1.05, 1e-12);
}

TEST(BinomialOptionPricer, EuropeanConvergesToBlackScholes) {
    BlackScholesAnalyticPricer bs;
    for (const MarketData& marketData :
         {MarketData {100.0, 0.05, 0.2}, MarketData {90.0, 0.03, 0.3, 0.02}}) {
        EuropeanCallOption call {100.0, 1.0};
        EuropeanPutOption put {100.0, 1.0};
        auto pricer = crrPricer(marketData, 1.0);
        EXPECT_NEAR(pricer.price(call, marketData).price, bs.price(call, marketData).price, 1e-2);
        EXPECT_NEAR(pricer.price(put, marketData).price, bs.price(put, marketData).price, 1e-2);
    }
}

TEST(BinomialOptionPricer, AmericanPutMatchesReference) {
    // Widely quoted reference value for S = K = 100, r = 5%, sigma = 20%, T = 1.
    constexpr MarketData marketData {100.0, 0.05, 0.2};
    AmericanPutOption put {100.0, 1.0};
    EXPECT_NEAR(crrPricer(marketData, 1.0).price(put, marketData).price, 6.0904, 5e-3);
}

TEST(BinomialOptionPricer, AmericanCallWithoutDividendsEqualsEuropean) {
    constexpr MarketData marketData {100.0, 0.05, 0.2};
    AmericanCallOption american {100.0, 1.0};
    EuropeanCallOption european {100.0, 1.0};
    auto pricer = crrPricer(marketData, 1.0);
    EXPECT_NEAR(pricer.price(american, marketData).price, pricer.price(european, marketData).price,
                1e-10);
}

TEST(BinomialOptionPricer, AmericanCallWithDividendsCarriesEarlyExercisePremium) {
    constexpr MarketData marketData {100.0, 0.05, 0.2, 0.08};
    AmericanCallOption american {100.0, 1.0};
    EuropeanCallOption european {100.0, 1.0};
    auto pricer = crrPricer(marketData, 1.0);
    EXPECT_GT(pricer.price(american, marketData).price,
              pricer.price(european, marketData).price + 1e-2);
}

} // namespace
