#include "gtest/gtest.h"

#include "scc/ADT/BaseVector.h"
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

} // namespace

TEST(BaseVectorTest, StoresWithinBaseCapacity) {
    BaseVector<int, 4, BasicAllocator> Vec;

    for (int I = 0; I < 4; ++I)
        Vec.pushBack(I);

    ASSERT_EQ(Vec.size(), 4u);
    EXPECT_EQ(Vec.capacity(), 4u);
    for (int I = 0; I < 4; ++I)
        EXPECT_EQ(Vec[I], I);
}

TEST(BaseVectorTest, GrowsBeyondBaseCapacity) {
    BaseVector<int, 2, BasicAllocator> Vec;

    Vec.pushBack(10);
    Vec.pushBack(20);
    Vec.pushBack(30);

    EXPECT_GE(Vec.capacity(), 4u);
    ASSERT_EQ(Vec.size(), 3u);
    EXPECT_EQ(Vec[0], 10);
    EXPECT_EQ(Vec[1], 20);
    EXPECT_EQ(Vec[2], 30);
}

TEST(BaseVectorTest, ManagesMoveOnlyTypesAndDestroysElements) {
    EXPECT_EQ(MoveOnlyCounter::Alive, 0);
    {
        BaseVector<MoveOnlyCounter, 2, BasicAllocator> Vec;
        Vec.pushBack(MoveOnlyCounter(1));
        Vec.pushBack(MoveOnlyCounter(2));
        Vec.pushBack(MoveOnlyCounter(3));
        EXPECT_EQ(Vec.size(), 3u);
        EXPECT_GE(Vec.capacity(), 4u);
        EXPECT_EQ(MoveOnlyCounter::Alive, 3);

        Vec.popBack();
        EXPECT_EQ(Vec.size(), 2u);
        EXPECT_EQ(MoveOnlyCounter::Alive, 2);
    }
    EXPECT_EQ(MoveOnlyCounter::Alive, 0);
}

TEST(BaseVectorTest, AcceptsAllocatorArguments) {
    int liveAllocators = 0;
    {
        BaseVector<int, 2, CountingAllocator> Vec(liveAllocators);
        Vec.pushBack(123);
        EXPECT_EQ(liveAllocators, 1);
    }
    EXPECT_EQ(liveAllocators, 0);
}
