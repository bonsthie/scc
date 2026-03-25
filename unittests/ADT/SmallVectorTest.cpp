#include "gtest/gtest.h"

#include "scc/ADT/SmallVector.h"
#include "scc/Allocator/BasicAllocator.h"

using namespace scc;

namespace {

class CountingAllocator : public BasicAllocator {
  public:
    explicit CountingAllocator(int &liveCounter) : Counter(&liveCounter) { ++*Counter; }
    CountingAllocator(const CountingAllocator &) = delete;
    CountingAllocator &operator=(const CountingAllocator &) = delete;
    ~CountingAllocator() override {
        if (Counter)
            --*Counter;
    }

  private:
    int *Counter;
};

} // namespace

TEST(SmallVectorTest, StoresWithinInlineCapacity) {
    SmallVector<int, 4> Vec;

    for (int I = 0; I < 4; ++I)
        Vec.push_back(I);

    ASSERT_EQ(Vec.size(), 4u);
    EXPECT_EQ(Vec.capacity(), 4u);
    for (int I = 0; I < 4; ++I)
        EXPECT_EQ(Vec[I], I);
}

TEST(SmallVectorTest, GrowsBeyondInlineCapacity) {
    SmallVector<int, 2> Vec;

    Vec.push_back(10);
    Vec.push_back(20);
    Vec.push_back(30);

    EXPECT_GE(Vec.capacity(), 4u);
    ASSERT_EQ(Vec.size(), 3u);
    EXPECT_EQ(Vec[0], 10);
    EXPECT_EQ(Vec[1], 20);
    EXPECT_EQ(Vec[2], 30);
}

struct MoveOnlyCounter {
    static int Alive;
    int        Value;

    explicit MoveOnlyCounter(int V = 0) : Value(V) { ++Alive; }
    MoveOnlyCounter(MoveOnlyCounter &&Other) noexcept : Value(Other.Value) {
        Other.Value = -1;
        ++Alive;
    }
    MoveOnlyCounter &operator=(MoveOnlyCounter &&Other) noexcept {
        if (this != &Other) {
            Value = Other.Value;
            Other.Value = -1;
        }
        return *this;
    }
    ~MoveOnlyCounter() { --Alive; }

    MoveOnlyCounter(const MoveOnlyCounter &) = delete;
    MoveOnlyCounter &operator=(const MoveOnlyCounter &) = delete;
};

int MoveOnlyCounter::Alive = 0;

TEST(SmallVectorTest, ManagesMoveOnlyTypesAndDestroysElements) {
    EXPECT_EQ(MoveOnlyCounter::Alive, 0);
    {
        SmallVector<MoveOnlyCounter, 2> Vec;
        Vec.push_back(MoveOnlyCounter(1));
        Vec.push_back(MoveOnlyCounter(2));
        Vec.push_back(MoveOnlyCounter(3));
        EXPECT_EQ(Vec.size(), 3u);
        EXPECT_GE(Vec.capacity(), 4u);
        EXPECT_EQ(MoveOnlyCounter::Alive, 3);

        Vec.pop_back();
        EXPECT_EQ(Vec.size(), 2u);
        EXPECT_EQ(MoveOnlyCounter::Alive, 2);
    }
    EXPECT_EQ(MoveOnlyCounter::Alive, 0);
}

TEST(SmallVectorTest, AcceptsAllocatorArguments) {
    int liveAllocators = 0;
    {
        SmallVector<int, 2, CountingAllocator> Vec(liveAllocators);
        Vec.push_back(123);
        EXPECT_EQ(liveAllocators, 1);
    }
    EXPECT_EQ(liveAllocators, 0);
}
