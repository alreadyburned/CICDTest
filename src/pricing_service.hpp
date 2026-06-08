#pragma once

#include "domain.hpp"

#include <functional>
#include <string>
#include <vector>

namespace cicd {

class PricingService {
public:
    PricingService(int tax_rate_percent, int discount_threshold, int discount_percent);

    int calculateSubtotal(const std::vector<CartLine>& lines,
                          const std::function<int(const std::string&)>& unitPriceProvider) const;
    int calculateDiscount(int subtotal) const;
    int calculateTax(int taxableAmount) const;
    int calculateTotal(int subtotal) const;

private:
    int tax_rate_percent_;
    int discount_threshold_;
    int discount_percent_;
};

}  // namespace cicd
