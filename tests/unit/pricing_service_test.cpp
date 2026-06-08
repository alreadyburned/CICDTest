#include "pricing_service.hpp"

#include <gtest/gtest.h>

#include <unordered_map>
#include <vector>

namespace {

TEST(UnitPricingServiceTest, CalculatesSubtotalWithValidItems) {
    cicd::PricingService pricing(10, 20000, 10);
    const std::vector<cicd::CartLine> lines = {
        {"A100", 2},
        {"B200", 1}
    };

    const std::unordered_map<std::string, int> prices = {
        {"A100", 7000},
        {"B200", 9000}
    };

    const int subtotal = pricing.calculateSubtotal(
        lines, [&prices](const std::string& sku) { return prices.at(sku); });

    EXPECT_EQ(subtotal, 23000);
}

TEST(UnitPricingServiceTest, AppliesDiscountOnlyAtThresholdOrAbove) {
    cicd::PricingService pricing(10, 20000, 10);

    EXPECT_EQ(pricing.calculateDiscount(19999), 0);
    EXPECT_EQ(pricing.calculateDiscount(20000), 2000);
}

TEST(UnitPricingServiceTest, CalculatesTotalFromSubtotal) {
    cicd::PricingService pricing(10, 20000, 10);

    const int total = pricing.calculateTotal(30000);

    EXPECT_EQ(total, 29700);  // 30000 - 3000 + 2700
}

}  // namespace
