#include "checkout_service.hpp"
#include "in_memory_inventory_repository.hpp"
#include "pricing_service.hpp"

#include <gtest/gtest.h>

#include <unordered_map>
#include <vector>

namespace {

TEST(IntegrationCheckoutTest, SuccessfulOrderConsumesStockAndReturnsReceipt) {
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

}  // namespace
