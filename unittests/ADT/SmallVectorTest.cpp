#include "gtest/gtest.h"

#include "scc/ADT/vector.h"

using namespace scc;

TEST(VectorTest, BasicOperations) {
    vector<int, 2> Vec;

    Vec.push_back(10);
    Vec.push_back(20);
    Vec.push_back(30);

    ASSERT_EQ(Vec.size(), 3u);
    EXPECT_GE(Vec.capacity(), 4u);
    EXPECT_EQ(Vec.front(), 10);
    EXPECT_EQ(Vec.back(), 30);

    Vec.pop_back();
    EXPECT_EQ(Vec.size(), 2u);
    EXPECT_EQ(Vec.back(), 20);
}

TEST(SmallVectorTest, PreservesInlineCapacityForSmallSizes) {
    SmallVector<int, 4> Vec;

    for (int I = 0; I < 4; ++I)
        Vec.emplace_back(I);

    ASSERT_EQ(Vec.size(), 4u);
    EXPECT_EQ(Vec.capacity(), 4u);
    for (int I = 0; I < 4; ++I)
        EXPECT_EQ(Vec[I], I);

    Vec.emplace_back(99);
    EXPECT_EQ(Vec.size(), 5u);
    EXPECT_EQ(Vec.back(), 99);
}
