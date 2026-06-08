#pragma once

#include "inventory_repository.hpp"

#include <string>
#include <unordered_map>

namespace cicd {

class InMemoryInventoryRepository : public InventoryRepository {
public:
    void setStock(const std::string& sku, int quantity) override;
    bool hasStock(const std::string& sku, int quantity) const override;
    bool reserve(const std::string& sku, int quantity) override;
    void release(const std::string& sku, int quantity) override;
    int available(const std::string& sku) const override;

private:
    std::unordered_map<std::string, int> stock_;
};

}  // namespace cicd
