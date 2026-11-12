//
// Created by Kin on 10/30/2026.
//

#include <cmath>
#include <gtest/gtest.h>

#include "option-pricer/option/EuropeanOption.hpp"
#include "option-pricer/pricer/BlackScholesAnalyticPricer.hpp"

namespace {

constexpr double kTolerance = 1e-10;

TEST(BlackScholesAnalyticPricer, AtTheMoneyCallMatchesReference) {
    EuropeanCallOption call {100.0, 1.0};
    BlackScholesAnalyticPricer pricer;
    EXPECT_NEAR(pricer.price(call, {100.0, 0.05, 0.2}).price, 10.450583572185565, kTolerance);
}

TEST(BlackScholesAnalyticPricer, AtTheMoneyPutMatchesReference) {
    EuropeanPutOption put {100.0, 1.0};
    BlackScholesAnalyticPricer pricer;
    EXPECT_NEAR(pricer.price(put, {100.0, 0.05, 0.2}).price, 5.573526022256971, kTolerance);
}

TEST(BlackScholesAnalyticPricer, DividendYieldMatchesReference) {
    EuropeanCallOption call {100.0, 0.5};
    EuropeanPutOption put {100.0, 0.5};
    BlackScholesAnalyticPricer pricer;
    constexpr MarketData marketData {90.0, 0.03, 0.3, 0.02};
    EXPECT_NEAR(pricer.price(call, marketData).price, 4.087050009394993, kTolerance);
    EXPECT_NEAR(pricer.price(put, marketData).price, 13.493758932276123, kTolerance);
}

TEST(BlackScholesAnalyticPricer, SatisfiesPutCallParityWithDividends) {
    constexpr double K = 105.0, T = 2.0;
    constexpr MarketData marketData {100.0, 0.04, 0.25, 0.03};
    EuropeanCallOption call {K, T};
    EuropeanPutOption put {K, T};
    BlackScholesAnalyticPricer pricer;

    const double lhs = pricer.price(call, marketData).price - pricer.price(put, marketData).price;
    const double rhs = marketData.spotPrice * std::exp(-marketData.dividendYield * T) -
                       K * std::exp(-marketData.riskFreeRate * T);
    EXPECT_NEAR(lhs, rhs, kTolerance);
}

TEST(BlackScholesAnalyticPricer, DividendsLowerCallAndRaisePut) {
    EuropeanCallOption call {100.0, 1.0};
    EuropeanPutOption put {100.0, 1.0};
    BlackScholesAnalyticPricer pricer;
    constexpr MarketData noDividend {100.0, 0.05, 0.2};
    constexpr MarketData withDividend {100.0, 0.05, 0.2, 0.03};
    EXPECT_LT(pricer.price(call, withDividend).price, pricer.price(call, noDividend).price);
    EXPECT_GT(pricer.price(put, withDividend).price, pricer.price(put, noDividend).price);
}

} // namespace
