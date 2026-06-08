#include "checkout_service.hpp"

#include <cstdio>
#include <utility>

namespace cicd {

CheckoutService::CheckoutService(InventoryRepository& inventory,
                                 PricingService pricing,
                                 std::unordered_map<std::string, int> unit_prices)
    : inventory_(inventory), pricing_(pricing), unit_prices_(std::move(unit_prices)) {}

CheckoutResult CheckoutService::placeOrder(const std::vector<CartLine>& lines) {
    if (lines.empty()) {
        return {false, "", 0, 0, 0, 0, "empty cart"};
    }

    std::vector<CartLine> reserved;
    reserved.reserve(lines.size());

    for (const auto& line : lines) {
        if (line.quantity <= 0) {
            for (const auto& r : reserved) {
                inventory_.release(r.sku, r.quantity);
            }
            return {false, "", 0, 0, 0, 0, "invalid quantity"};
        }

        const auto price_it = unit_prices_.find(line.sku);
        if (price_it == unit_prices_.end()) {
            for (const auto& r : reserved) {
                inventory_.release(r.sku, r.quantity);
            }
            return {false, "", 0, 0, 0, 0, "unknown sku"};
        }

        if (!inventory_.reserve(line.sku, line.quantity)) {
            for (const auto& r : reserved) {
                inventory_.release(r.sku, r.quantity);
            }
            return {false, "", 0, 0, 0, 0, "out of stock"};
        }
        reserved.push_back(line);
    }

    const int subtotal = pricing_.calculateSubtotal(
        lines,
        [this](const std::string& sku) { return unit_prices_.at(sku); });

    const int discount = pricing_.calculateDiscount(subtotal);
    const int tax = pricing_.calculateTax(subtotal - discount);
    const int total = subtotal - discount + tax;

    CheckoutResult result;
    result.success = true;
    result.order_id = generateOrderId();
    result.subtotal = subtotal;
    result.discount = discount;
    result.tax = tax;
    result.total = total;
    result.message = "ok";
    return result;
}

std::string CheckoutService::generateOrderId() {
    char buffer[16] = {0};
    std::snprintf(buffer, sizeof(buffer), "ORD-%06d", next_order_number_++);
    return buffer;
}

}  // namespace cicd
