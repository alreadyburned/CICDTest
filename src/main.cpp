#include "checkout_service.hpp"
#include "domain.hpp"
#include "in_memory_inventory_repository.hpp"
#include "pricing_service.hpp"

#include <iostream>
#include <unordered_map>
#include <vector>

int main() {
    cicd::InMemoryInventoryRepository inventory;
    inventory.setStock("A100", 10);
    inventory.setStock("B200", 5);

    cicd::PricingService pricing(10, 20000, 10);  // 10% tax, 10% discount over 200.00
    std::unordered_map<std::string, int> prices = {
        {"A100", 7000},  // 70.00
        {"B200", 9000}   // 90.00
    };

    cicd::CheckoutService checkout(inventory, pricing, prices);
    std::vector<cicd::CartLine> cart = {
        {"A100", 2},
        {"B200", 1}
    };

    const cicd::CheckoutResult result = checkout.placeOrder(cart);
    std::cout << "success: " << result.success << "\n";
    std::cout << "order id: " << result.order_id << "\n";
    std::cout << "total: " << result.total << " cents\n";
    return result.success ? 0 : 1;
}
