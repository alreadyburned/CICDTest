#include "pricing_service.hpp"

#include <gtest/gtest.h>

#include <unordered_map>
#include <vector>

namespace {

TEST(UnitPricingServiceTest, CalculatesSubtotalWithValidItems) {
    // 의도: 정상 입력에서 상품별 합계가 정확히 계산되는지 검증한다.
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
    // 의도: 할인 임계값 경계에서 할인 적용 여부가 올바른지 확인한다.
    cicd::PricingService pricing(10, 20000, 10);

    EXPECT_EQ(pricing.calculateDiscount(19999), 0);
    EXPECT_EQ(pricing.calculateDiscount(20000), 2000);
}

TEST(UnitPricingServiceTest, CalculatesTotalFromSubtotal) {
    // 의도: subtotal -> discount -> tax -> total 계산 흐름을 한 번에 검증한다.
    cicd::PricingService pricing(10, 20000, 10);

    const int total = pricing.calculateTotal(30000);

    EXPECT_EQ(total, 29700);  // 30000 - 3000 + 2700
}

TEST(UnitPricingServiceTest, IgnoresNonPositiveQuantitiesInSubtotal) {
    // 의도: 수량이 0 이하인 라인은 subtotal 계산에서 제외되는지 확인한다.
    cicd::PricingService pricing(10, 20000, 10);
    const std::vector<cicd::CartLine> lines = {
        {"A100", 2},
        {"B200", 0},
        {"C300", -3}
    };

    const std::unordered_map<std::string, int> prices = {
        {"A100", 7000},
        {"B200", 9000},
        {"C300", 11000}
    };

    const int subtotal = pricing.calculateSubtotal(
        lines, [&prices](const std::string& sku) { return prices.at(sku); });

    EXPECT_EQ(subtotal, 14000);
}

TEST(UnitPricingServiceTest, ReturnsZeroTaxForNonPositiveTaxableAmount) {
    // 의도: 과세 대상 금액이 0 이하일 때 세금이 0으로 처리되는지 검증한다.
    cicd::PricingService pricing(10, 20000, 10);

    EXPECT_EQ(pricing.calculateTax(0), 0);
    EXPECT_EQ(pricing.calculateTax(-1), 0);
}

TEST(UnitPricingServiceTest, CalculatesTotalWithoutDiscountBelowThreshold) {
    // 의도: 할인 임계값 미만에서는 total이 subtotal + tax로 계산되는지 확인한다.
    cicd::PricingService pricing(10, 20000, 10);

    const int total = pricing.calculateTotal(10000);

    EXPECT_EQ(total, 11000);
}

}  // namespace
