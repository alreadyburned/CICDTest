#pragma once

#include <string>

namespace cicd {

class InventoryRepository {
public:
    virtual ~InventoryRepository() = default;

    virtual void setStock(const std::string& sku, int quantity) = 0;
    virtual bool hasStock(const std::string& sku, int quantity) const = 0;
    virtual bool reserve(const std::string& sku, int quantity) = 0;
    virtual void release(const std::string& sku, int quantity) = 0;
    virtual int available(const std::string& sku) const = 0;
};

}  // namespace cicd
