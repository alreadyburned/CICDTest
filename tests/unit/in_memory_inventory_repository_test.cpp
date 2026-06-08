#include "in_memory_inventory_repository.hpp"

#include <gtest/gtest.h>

namespace {

TEST(UnitInventoryRepositoryTest, ReserveReducesStock) {
    cicd::InMemoryInventoryRepository repo;
    repo.setStock("A100", 5);

    EXPECT_TRUE(repo.reserve("A100", 3));
    EXPECT_EQ(repo.available("A100"), 2);
}

TEST(UnitInventoryRepositoryTest, ReserveFailsWhenInsufficient) {
    cicd::InMemoryInventoryRepository repo;
    repo.setStock("A100", 2);

    EXPECT_FALSE(repo.reserve("A100", 3));
    EXPECT_EQ(repo.available("A100"), 2);
}

TEST(UnitInventoryRepositoryTest, ReleaseRestoresStock) {
    cicd::InMemoryInventoryRepository repo;
    repo.setStock("A100", 3);

    ASSERT_TRUE(repo.reserve("A100", 2));
    repo.release("A100", 2);

    EXPECT_EQ(repo.available("A100"), 3);
}

}  // namespace
