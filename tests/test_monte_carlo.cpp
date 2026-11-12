//
// Created by Kin on 11/12/2026.
//

#include <gtest/gtest.h>

#include "option-pricer/option/EuropeanOption.hpp"
#include "option-pricer/pricer/BlackScholesAnalyticPricer.hpp"
#include "option-pricer/pricer/MonteCarloSimulationPricer.hpp"

namespace {

TEST(MonteCarloSimulationPricer, EuropeanWithinFourStandardErrorsOfBlackScholes) {
    BlackScholesAnalyticPricer bs;
    for (const MarketData& marketData :
         {MarketData {100.0, 0.05, 0.2}, MarketData {90.0, 0.03, 0.3, 0.02}}) {
        EuropeanCallOption call {100.0, 1.0};
        EuropeanPutOption put {100.0, 1.0};
        MonteCarloSimulationPricer mc {200000, 42};

        const PricerResult callResult = mc.price(call, marketData);
        const PricerResult putResult = mc.price(put, marketData);
        ASSERT_TRUE(callResult.standardError.has_value());
        ASSERT_TRUE(putResult.standardError.has_value());
        EXPECT_NEAR(callResult.price, bs.price(call, marketData).price,
                    4.0 * *callResult.standardError);
        EXPECT_NEAR(putResult.price, bs.price(put, marketData).price,
                    4.0 * *putResult.standardError);
    }
}

TEST(MonteCarloSimulationPricer, SameSeedIsDeterministic) {
    EuropeanCallOption call {100.0, 1.0};
    constexpr MarketData marketData {100.0, 0.05, 0.2};
    MonteCarloSimulationPricer first {10000, 7};
    MonteCarloSimulationPricer second {10000, 7};
    EXPECT_DOUBLE_EQ(first.price(call, marketData).price, second.price(call, marketData).price);
}

} // namespace
