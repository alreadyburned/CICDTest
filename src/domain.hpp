#pragma once

#include <string>
#include <vector>

namespace cicd {

struct CartLine {
    std::string sku;
    int quantity;
};

struct CheckoutResult {
    bool success = false;
    std::string order_id;
    int subtotal = 0;
    int discount = 0;
    int tax = 0;
    int total = 0;
    std::string message;
};

}  // namespace cicd
