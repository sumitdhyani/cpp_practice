#include <gtest/gtest.h>
#include <vector>
#include <CustomAllocatorSegmentTree.hpp>// Include the header for the SegmentTree class

// Test fixture class for the SegmentTree
class SegmentTreeTest : public ::testing::Test {
protected:
    std::vector<int> arr;
    SegmentTree<int, 8> *tree;

    // Function to initialize the SegmentTree with the array 'arr'
    virtual void SetUp() override {
        arr = {1, 5, 3, 7, 9, 2, 6, 4};
        tree = new SegmentTree<int, 8>([&](uint32_t idx) { return arr[idx]; });
        tree->build();
    }

    virtual void TearDown() override {
        delete tree;
    }
};

// Test the initial construction and correct index of the maximum element
TEST_F(SegmentTreeTest, InitializationTest) {
    // Test that the maximum element in the full range is at index 4 (value 9)
    EXPECT_EQ(tree->maxInrange(0, 7), 4);
}

// Test querying for the maximum element index in different ranges
TEST_F(SegmentTreeTest, RangeMaxQueryTest) {
    // Test some different ranges
    EXPECT_EQ(tree->maxInrange(0, 3), 3); // max in {1, 5, 3, 7} -> index 3 (value 7)
    EXPECT_EQ(tree->maxInrange(2, 5), 4); // max in {3, 7, 9, 2} -> index 4 (value 9)
    EXPECT_EQ(tree->maxInrange(5, 7), 6); // max in {2, 6, 4} -> index 6 (value 6)
}

// Test updating an element and verifying the max index is updated correctly
TEST_F(SegmentTreeTest, UpdateTest) {
    // Initially, max in the full range is at index 4 (value 9)
    EXPECT_EQ(tree->maxInrange(0, 7), 4);

    // Update arr[4] to 1, making the max value in the range change
    arr[4] = 1;
    tree->update(4, arr[4]);

    // Now the maximum should be at index 3 (value 7)
    EXPECT_EQ(tree->maxInrange(0, 7), 3);

    // Update arr[7] to 10, which should now be the maximum value in the entire range
    arr[7] = 10;
    tree->update(7, arr[7]);

    // Now the maximum should be at index 7 (value 10)
    EXPECT_EQ(tree->maxInrange(0, 7), 7);
}

// Test edge cases
TEST_F(SegmentTreeTest, EdgeCaseTest) {
    // Check single element range
    EXPECT_EQ(tree->maxInrange(4, 4), 4); // Only one element at index 4

    // Check out of range query (should not crash or throw)
    EXPECT_EQ(tree->maxInrange(8, 9), Size_max); // Out of bounds range
}

// Main function for running the tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}