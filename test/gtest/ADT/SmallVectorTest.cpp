#include "gtest/gtest.h"

#include "scc/ADT/vector.h"
#include <memory>
#include <type_traits>

using namespace scc;

TEST(VectorTest, BasicOperations) {
    Vector<int, 2> Vec;

    Vec.pushBack(10);
    Vec.pushBack(20);
    Vec.pushBack(30);

    ASSERT_EQ(Vec.size(), 3u);
    EXPECT_GE(Vec.capacity(), 4u);
    EXPECT_EQ(Vec.front(), 10);
    EXPECT_EQ(Vec.back(), 30);

    Vec.popBack();
    EXPECT_EQ(Vec.size(), 2u);
    EXPECT_EQ(Vec.back(), 20);
}

TEST(SmallVectorTest, PreservesInlineCapacityForSmallSizes) {
    SmallVector<int, 4> Vec;

    for (int I = 0; I < 4; ++I)
        Vec.emplaceBack(I);

    ASSERT_EQ(Vec.size(), 4u);
    EXPECT_EQ(Vec.capacity(), 4u);
    for (int I = 0; I < 4; ++I)
        EXPECT_EQ(Vec[I], I);

    Vec.emplaceBack(99);
    EXPECT_EQ(Vec.size(), 5u);
    EXPECT_EQ(Vec.back(), 99);
}

TEST(VectorTest, SupportsMoveOnlyElements) {
    static_assert(!std::is_copy_constructible_v<Vector<std::unique_ptr<int>, 2>>);

    Vector<std::unique_ptr<int>, 2> Vec;
    Vec.pushBack(std::make_unique<int>(10));
    Vec.pushBack(std::make_unique<int>(20));
    Vec.pushBack(std::make_unique<int>(30));

    ASSERT_EQ(Vec.size(), 3u);
    EXPECT_EQ(*Vec[0], 10);
    EXPECT_EQ(*Vec[1], 20);
    EXPECT_EQ(*Vec[2], 30);
}
