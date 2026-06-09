#include "in_memory_inventory_repository.hpp"

#include <gtest/gtest.h>

namespace {

TEST(UnitInventoryRepositoryTest, ReserveReducesStock) {
    // 의도: reserve 성공 시 재고가 요청 수량만큼 감소하는지 확인한다.
    cicd::InMemoryInventoryRepository repo;
    repo.setStock("A100", 5);

    EXPECT_TRUE(repo.reserve("A100", 3));
    EXPECT_EQ(repo.available("A100"), 2);
}


// TEST(UnitInventoryRepositoryTest_FailureExample, ReserveReducesStock) {
//     // 의도: 테스트 실패 예시. (항상 실패하게 되어 있음.)
//     cicd::InMemoryInventoryRepository repo;
//     repo.setStock("A100", 5);

//     EXPECT_TRUE(repo.reserve("A100", 100));
//     EXPECT_EQ(repo.available("A100"), 2);
// }


TEST(UnitInventoryRepositoryTest, ReserveFailsWhenInsufficient) {
    // 의도: 재고 부족 시 reserve가 실패하고 재고가 유지되는지 검증한다.
    cicd::InMemoryInventoryRepository repo;
    repo.setStock("A100", 2);

    EXPECT_FALSE(repo.reserve("A100", 3));
    EXPECT_EQ(repo.available("A100"), 2);
}

TEST(UnitInventoryRepositoryTest, ReleaseRestoresStock) {
    // 의도: 이미 예약된 수량을 release하면 재고가 복구되는지 검증한다.
    cicd::InMemoryInventoryRepository repo;
    repo.setStock("A100", 3);

    ASSERT_TRUE(repo.reserve("A100", 2));
    repo.release("A100", 2);

    EXPECT_EQ(repo.available("A100"), 3);
}

TEST(UnitInventoryRepositoryTest, SetStockClampsNegativeToZero) {
    // 의도: 음수 재고 입력이 0으로 보정되는지 확인한다.
    cicd::InMemoryInventoryRepository repo;

    repo.setStock("A100", -5);

    EXPECT_EQ(repo.available("A100"), 0);
}

TEST(UnitInventoryRepositoryTest, HasStockReturnsFalseForNonPositiveRequest) {
    // 의도: 요청 수량이 0 이하인 경우 재고 보유 판단을 false로 처리하는지 확인한다.
    cicd::InMemoryInventoryRepository repo;
    repo.setStock("A100", 10);

    EXPECT_FALSE(repo.hasStock("A100", 0));
    EXPECT_FALSE(repo.hasStock("A100", -1));
}

TEST(UnitInventoryRepositoryTest, ReleaseIgnoresNonPositiveQuantity) {
    // 의도: 0 이하 수량 release 호출이 재고를 변경하지 않는지 검증한다.
    cicd::InMemoryInventoryRepository repo;
    repo.setStock("A100", 3);

    repo.release("A100", 0);
    repo.release("A100", -2);

    EXPECT_EQ(repo.available("A100"), 3);
}

TEST(UnitInventoryRepositoryTest, ReserveFailsForUnknownSku) {
    // 의도: 존재하지 않는 SKU에 대한 reserve 요청이 실패하는지 확인한다.
    cicd::InMemoryInventoryRepository repo;

    EXPECT_FALSE(repo.reserve("UNKNOWN", 1));
    EXPECT_EQ(repo.available("UNKNOWN"), 0);
}

}  // namespace
