#include "pricing_service.hpp"

namespace cicd {

PricingService::PricingService(int tax_rate_percent, int discount_threshold, int discount_percent)
    : tax_rate_percent_(tax_rate_percent),
      discount_threshold_(discount_threshold),
      discount_percent_(discount_percent) {}

int PricingService::calculateSubtotal(const std::vector<CartLine>& lines,
                                      const std::function<int(const std::string&)>& unitPriceProvider) const {
    int subtotal = 0;
    for (const auto& line : lines) {
        if (line.quantity <= 0) {
            continue;
        }
        subtotal += unitPriceProvider(line.sku) * line.quantity;
    }
    return subtotal;
}

int PricingService::calculateDiscount(int subtotal) const {
    if (subtotal < discount_threshold_) {
        return 0;
    }
    return (subtotal * discount_percent_) / 100;
}

int PricingService::calculateTax(int taxableAmount) const {
    if (taxableAmount <= 0) {
        return 0;
    }
    return (taxableAmount * tax_rate_percent_) / 100;
}

int PricingService::calculateTotal(int subtotal) const {
    const int discount = calculateDiscount(subtotal);
    const int taxable = subtotal - discount;
    const int tax = calculateTax(taxable);
    return taxable + tax;
}

}  // namespace cicd
