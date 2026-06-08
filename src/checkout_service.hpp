#pragma once

#include "domain.hpp"
#include "inventory_repository.hpp"
#include "pricing_service.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace cicd {

class CheckoutService {
public:
    CheckoutService(InventoryRepository& inventory,
                    PricingService pricing,
                    std::unordered_map<std::string, int> unit_prices);

    CheckoutResult placeOrder(const std::vector<CartLine>& lines);

private:
    std::string generateOrderId();

    InventoryRepository& inventory_;
    PricingService pricing_;
    std::unordered_map<std::string, int> unit_prices_;
    int next_order_number_ = 1;
};

}  // namespace cicd
