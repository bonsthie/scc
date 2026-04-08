#include <gtest/gtest.h>

#include "scc/Allocator/StackGrowAllocator.h"

#include <cstddef>
#include <cstdint>

using namespace scc;

TEST(StackGrowAllocatorTest, UsesStackStorageUntilReset) {
    StackGrowAllocator<128> Alloc;

    EXPECT_EQ(Alloc.stackBytesUsed(), 0u);
    auto *First = Alloc.allocateArray<std::uint64_t>();
    ASSERT_NE(First, nullptr);
    EXPECT_GT(Alloc.stackBytesUsed(), 0u);
    auto AllocBegin = reinterpret_cast<std::uintptr_t>(&Alloc);
    auto AllocEnd = AllocBegin + sizeof(Alloc);
    auto FirstPtr = reinterpret_cast<std::uintptr_t>(First);
    EXPECT_GE(FirstPtr, AllocBegin);
    EXPECT_LT(FirstPtr + sizeof(std::uint64_t), AllocEnd);

    auto UsedAfterFirst = Alloc.stackBytesUsed();
    auto *Second = Alloc.allocateArray<std::uint64_t>(2);
    ASSERT_NE(Second, nullptr);
    EXPECT_GT(Alloc.stackBytesUsed(), UsedAfterFirst);

    Alloc.reset();
    EXPECT_EQ(Alloc.stackBytesUsed(), 0u);
}

TEST(StackGrowAllocatorTest, LargeAllocationsBypassStack) {
    StackGrowAllocator<32> Alloc;
    auto               usedBefore = Alloc.stackBytesUsed();

    auto *largeAlloc = Alloc.allocate_bytes(512, alignof(std::max_align_t));
    ASSERT_NE(largeAlloc, nullptr);
    EXPECT_EQ(Alloc.stackBytesUsed(), usedBefore);

    Alloc.deallocate_bytes(largeAlloc, 512);
}

TEST(StackGrowAllocatorTest, LifoDeallocateShrinksStackUsage) {
    StackGrowAllocator<256> Alloc;

    auto *First = Alloc.allocateArray<int>(2);
    auto *Second = Alloc.allocateArray<int>(4);
    auto *Third = Alloc.allocateArray<int>(1);
    ASSERT_NE(First, nullptr);
    ASSERT_NE(Second, nullptr);
    ASSERT_NE(Third, nullptr);

    auto UsedBeforeThirdFree = Alloc.stackBytesUsed();
    Alloc.deallocate_bytes(Third, sizeof(int) * 1);
    EXPECT_LT(Alloc.stackBytesUsed(), UsedBeforeThirdFree);

    auto UsedBeforeSecondFree = Alloc.stackBytesUsed();
    Alloc.deallocate_bytes(Second, sizeof(int) * 4);
    EXPECT_LT(Alloc.stackBytesUsed(), UsedBeforeSecondFree);

    auto UsedBeforeFirstFree = Alloc.stackBytesUsed();
    Alloc.deallocate_bytes(First, sizeof(int) * 2);
    EXPECT_LT(Alloc.stackBytesUsed(), UsedBeforeFirstFree);
    EXPECT_EQ(Alloc.stackBytesUsed(), 0u);
}

TEST(StackGrowAllocatorTest, NonLifoDeallocateDoesNothing) {
    StackGrowAllocator<128> Alloc;

    auto *First = Alloc.allocateArray<int>(1);
    auto *Second = Alloc.allocateArray<int>(1);
    ASSERT_NE(First, nullptr);
    ASSERT_NE(Second, nullptr);

    auto UsedBefore = Alloc.stackBytesUsed();
    Alloc.deallocate_bytes(First, sizeof(int) * 1);
    EXPECT_EQ(Alloc.stackBytesUsed(), UsedBefore);

    Alloc.reset();
    EXPECT_EQ(Alloc.stackBytesUsed(), 0u);
}

TEST(StackGrowAllocatorTest, ConstructorCanCapInlineStorageUsage) {
    StackGrowAllocator<256> Alloc(64);

    EXPECT_EQ(Alloc.stackBytesCapacity(), 64u);
}
