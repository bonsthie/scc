#include <gtest/gtest.h>

#include "scc/Allocator/BumpAllocator.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <string>

using namespace scc;

struct alignas(32) WideType {
    std::array<char, 32> Payload{};
};

TEST(BumpAllocatorTest, HonorsAlignmentAndRollsOverChunks) {
    // Force chunk rollover by using a tiny allocator.
    BumpAllocator Alloc(/*chunk_size=*/64);

    auto *First = Alloc.alloc<WideType>();
    auto *Second = Alloc.alloc<WideType>();

    ASSERT_NE(First, nullptr);
    ASSERT_NE(Second, nullptr);
    EXPECT_NE(First, Second);

    EXPECT_EQ(reinterpret_cast<uintptr_t>(First) % alignof(WideType), 0u);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(Second) % alignof(WideType), 0u);
}

TEST(BumpAllocatorTest, CopyAndAllocStringProduceIndependentStorage) {
    BumpAllocator Alloc(128);

    const char *Text = "hello world";
    void       *CopyPtr = Alloc.copy(Text, std::strlen(Text) + 1);
    ASSERT_NE(CopyPtr, nullptr);
    EXPECT_STREQ(static_cast<const char *>(CopyPtr), Text);

    std::string_view Interned = Alloc.allocString("sample");
    EXPECT_EQ(Interned, "sample");

    // Mutate the source buffer and make sure interned data is unaffected.
    std::string      Source = "mutate";
    std::string_view View = Alloc.allocString(Source);
    Source[0] = 'x';
    EXPECT_EQ(View, "mutate");
}

TEST(BumpAllocatorTest, ResetReusesExistingChunks) {
    BumpAllocator Alloc(64);

    int *First = Alloc.alloc<int>(42);
    ASSERT_NE(First, nullptr);
    uintptr_t FirstAddr = reinterpret_cast<uintptr_t>(First);

    Alloc.reset();

    int *Second = Alloc.alloc<int>(7);
    ASSERT_NE(Second, nullptr);
    uintptr_t SecondAddr = reinterpret_cast<uintptr_t>(Second);

    EXPECT_EQ(FirstAddr, SecondAddr);
    EXPECT_EQ(*Second, 7);
}
