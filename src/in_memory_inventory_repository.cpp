#include "in_memory_inventory_repository.hpp"

namespace cicd {

void InMemoryInventoryRepository::setStock(const std::string& sku, int quantity) {
    stock_[sku] = quantity < 0 ? 0 : quantity;
}

bool InMemoryInventoryRepository::hasStock(const std::string& sku, int quantity) const {
    if (quantity <= 0) {
        return false;
    }
    const auto it = stock_.find(sku);
    if (it == stock_.end()) {
        return false;
    }
    return it->second >= quantity;
}

bool InMemoryInventoryRepository::reserve(const std::string& sku, int quantity) {
    if (!hasStock(sku, quantity)) {
        return false;
    }
    stock_[sku] -= quantity;
    return true;
}

void InMemoryInventoryRepository::release(const std::string& sku, int quantity) {
    if (quantity <= 0) {
        return;
    }
    stock_[sku] += quantity;
}

int InMemoryInventoryRepository::available(const std::string& sku) const {
    const auto it = stock_.find(sku);
    if (it == stock_.end()) {
        return 0;
    }
    return it->second;
}

}  // namespace cicd
