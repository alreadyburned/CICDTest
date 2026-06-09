#include "checkout_service.hpp"
#include "in_memory_inventory_repository.hpp"
#include "pricing_service.hpp"

#include <gtest/gtest.h>

#include <unordered_map>
#include <vector>

namespace {

TEST(IntegrationCheckoutTest, SuccessfulOrderConsumesStockAndReturnsReceipt) {
    // 의도: 정상 주문 시 재고 차감과 영수증 계산 결과를 함께 검증한다.
    cicd::InMemoryInventoryRepository inventory;
    inventory.setStock("A100", 10);
    inventory.setStock("B200", 5);

    cicd::PricingService pricing(10, 20000, 10);
    std::unordered_map<std::string, int> prices = {
        {"A100", 7000},
        {"B200", 9000}
    };

    cicd::CheckoutService checkout(inventory, pricing, prices);

    const std::vector<cicd::CartLine> cart = {
        {"A100", 2},
        {"B200", 1}
    };

    const cicd::CheckoutResult result = checkout.placeOrder(cart);

    ASSERT_TRUE(result.success);
    EXPECT_EQ(result.subtotal, 23000);
    EXPECT_EQ(result.discount, 2300);
    EXPECT_EQ(result.tax, 2070);
    EXPECT_EQ(result.total, 22770);
    EXPECT_EQ(inventory.available("A100"), 8);
    EXPECT_EQ(inventory.available("B200"), 4);
    EXPECT_FALSE(result.order_id.empty());
}

TEST(IntegrationCheckoutTest, FailsAndRollsBackWhenStockIsInsufficient) {
    // 의도: 주문 중 재고 부족이 발생하면 이전 예약이 롤백되는지 확인한다.
    cicd::InMemoryInventoryRepository inventory;
    inventory.setStock("A100", 1);
    inventory.setStock("B200", 5);

    cicd::PricingService pricing(10, 20000, 10);
    std::unordered_map<std::string, int> prices = {
        {"A100", 7000},
        {"B200", 9000}
    };

    cicd::CheckoutService checkout(inventory, pricing, prices);

    const std::vector<cicd::CartLine> cart = {
        {"A100", 1},
        {"B200", 6}
    };

    const cicd::CheckoutResult result = checkout.placeOrder(cart);

    ASSERT_FALSE(result.success);
    EXPECT_EQ(result.message, "out of stock");
    EXPECT_EQ(inventory.available("A100"), 1);  // reserved stock rollback
    EXPECT_EQ(inventory.available("B200"), 5);
}

TEST(IntegrationCheckoutTest, FailsForEmptyCart) {
    // 의도: 빈 장바구니 주문이 즉시 실패하고 명확한 메시지를 반환하는지 확인한다.
    cicd::InMemoryInventoryRepository inventory;
    cicd::PricingService pricing(10, 20000, 10);
    std::unordered_map<std::string, int> prices = {
        {"A100", 7000}
    };
    cicd::CheckoutService checkout(inventory, pricing, prices);

    const std::vector<cicd::CartLine> cart;
    const cicd::CheckoutResult result = checkout.placeOrder(cart);

    ASSERT_FALSE(result.success);
    EXPECT_EQ(result.message, "empty cart");
}

TEST(IntegrationCheckoutTest, FailsOnInvalidQuantityAndRollsBackPriorReservations) {
    // 의도: 유효하지 않은 수량을 만나면 직전까지의 예약 재고가 원복되는지 검증한다.
    cicd::InMemoryInventoryRepository inventory;
    inventory.setStock("A100", 10);
    inventory.setStock("B200", 5);

    cicd::PricingService pricing(10, 20000, 10);
    std::unordered_map<std::string, int> prices = {
        {"A100", 7000},
        {"B200", 9000}
    };

    cicd::CheckoutService checkout(inventory, pricing, prices);
    const std::vector<cicd::CartLine> cart = {
        {"A100", 2},
        {"B200", 0}
    };

    const cicd::CheckoutResult result = checkout.placeOrder(cart);

    ASSERT_FALSE(result.success);
    EXPECT_EQ(result.message, "invalid quantity");
    EXPECT_EQ(inventory.available("A100"), 10);
    EXPECT_EQ(inventory.available("B200"), 5);
}

TEST(IntegrationCheckoutTest, FailsOnUnknownSkuAndRollsBackPriorReservations) {
    // 의도: 미등록 SKU를 만나면 실패하고 이전 SKU 예약이 롤백되는지 확인한다.
    cicd::InMemoryInventoryRepository inventory;
    inventory.setStock("A100", 10);

    cicd::PricingService pricing(10, 20000, 10);
    std::unordered_map<std::string, int> prices = {
        {"A100", 7000}
    };

    cicd::CheckoutService checkout(inventory, pricing, prices);
    const std::vector<cicd::CartLine> cart = {
        {"A100", 2},
        {"UNKNOWN", 1}
    };

    const cicd::CheckoutResult result = checkout.placeOrder(cart);

    ASSERT_FALSE(result.success);
    EXPECT_EQ(result.message, "unknown sku");
    EXPECT_EQ(inventory.available("A100"), 10);
}

TEST(IntegrationCheckoutTest, GeneratesIncrementingOrderIdsForSuccessfulOrders) {
    // 의도: 성공 주문이 연속 발생할 때 주문번호가 순차 증가하는지 검증한다.
    cicd::InMemoryInventoryRepository inventory;
    inventory.setStock("A100", 10);

    cicd::PricingService pricing(10, 20000, 10);
    std::unordered_map<std::string, int> prices = {
        {"A100", 7000}
    };

    cicd::CheckoutService checkout(inventory, pricing, prices);
    const std::vector<cicd::CartLine> cart = {
        {"A100", 1}
    };

    const cicd::CheckoutResult first = checkout.placeOrder(cart);
    const cicd::CheckoutResult second = checkout.placeOrder(cart);

    ASSERT_TRUE(first.success);
    ASSERT_TRUE(second.success);
    EXPECT_EQ(first.order_id, "ORD-000001");
    EXPECT_EQ(second.order_id, "ORD-000002");
}

}  // namespace
