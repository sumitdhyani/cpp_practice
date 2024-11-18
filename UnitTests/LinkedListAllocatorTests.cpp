#include <gtest/gtest.h>
#include <CustomAllocator.hpp>

// Test fixture class for ResourceManager
class ResourceManagerTest : public ::testing::Test {
protected:
    // Define the memory size for testing purposes
    static constexpr uint32_t TEST_SIZE = 10;

    // Use the concrete implementation of ResourceManager for testing
    FreeResourcetable<TEST_SIZE> resourceManager;
};

// Test allocation of a single block of memory
TEST_F(ResourceManagerTest, AllocateSingleBlock) {
    auto idx = resourceManager.getNextFreeIdx(3);
    EXPECT_EQ(idx.value(), 0) << "Expected allocation at index 0";

    idx = resourceManager.getNextFreeIdx(2);
    EXPECT_EQ(idx.value(), 3) << "Expected allocation at index 3";
}

// Test allocation after freeing a block
TEST_F(ResourceManagerTest, AllocateAfterFree) {
    auto idx = resourceManager.getNextFreeIdx(4);
    EXPECT_EQ(idx.value(), 0) << "Expected allocation at index 0";

    resourceManager.freeIdx(0);

    idx = resourceManager.getNextFreeIdx(2);
    EXPECT_EQ(idx.value(), 0) << "Expected reallocation at index 0 after freeing";
}

// Test that allocation fails if not enough space is available
TEST_F(ResourceManagerTest, AllocationFailsIfNotEnoughSpace) {
    resourceManager.getNextFreeIdx(7);  // Occupy 7 slots
    auto idx = resourceManager.getNextFreeIdx(4);  // Try to allocate 4 slots
    EXPECT_EQ(idx.has_value(), false) << "Expected failure (index = size) due to insufficient space";
}

// Test that adjacent blocks can be allocated correctly
TEST_F(ResourceManagerTest, AllocateAdjacentBlocks) {
    auto idx1 = resourceManager.getNextFreeIdx(3);
    EXPECT_EQ(idx1.value(), 0) << "Expected allocation at index 0";

    auto idx2 = resourceManager.getNextFreeIdx(3);
    EXPECT_EQ(idx2.value(), 3) << "Expected allocation at index 3";

    auto idx3 = resourceManager.getNextFreeIdx(3);
    EXPECT_EQ(idx3.value(), 6) << "Expected allocation at index 6";
}

// Test that freeing a block allows for reallocation
TEST_F(ResourceManagerTest, FreeAndReallocate) {
    auto idx1 = resourceManager.getNextFreeIdx(5);
    EXPECT_EQ(idx1.value(), 0) << "Expected allocation at index 0";

    resourceManager.freeIdx(0);

    auto idx2 = resourceManager.getNextFreeIdx(5);
    EXPECT_EQ(idx2.value(), 0) << "Expected reallocation at index 0 after freeing";
}

// Test complex allocation and free pattern
TEST_F(ResourceManagerTest, ComplexAllocationPattern) {
    auto idx1 = resourceManager.getNextFreeIdx(3);
    EXPECT_EQ(idx1.value(), 0) << "Expected allocation at index 0";

    auto idx2 = resourceManager.getNextFreeIdx(4);
    EXPECT_EQ(idx2.value(), 3) << "Expected allocation at index 3";

    resourceManager.freeIdx(0);  // Free the first block

    auto idx3 = resourceManager.getNextFreeIdx(2);
    EXPECT_EQ(idx3.value(), 0) << "Expected reallocation at index 0 after freeing part of memory";

    auto idx4 = resourceManager.getNextFreeIdx(3);
    EXPECT_EQ(idx4.value(), 7) << "Expected allocation at index 7 for the last block";
}
