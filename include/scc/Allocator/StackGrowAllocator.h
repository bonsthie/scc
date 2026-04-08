#ifndef SCC_ALLOCATOR_STACK_GROW_ALLOCATOR_H
#define SCC_ALLOCATOR_STACK_GROW_ALLOCATOR_H

#include <cstddef>
#include <memory>

#include "scc/Allocator/Allocator.h"
#include "scc/Allocator/BasicAllocator.h"

namespace scc {

template <size_t DefaultStackBytes = 4096, size_t AlignSize = alignof(std::max_align_t)>
class StackGrowAllocator : public Allocator {
  public:
    static_assert(DefaultStackBytes > 0, "StackGrowAllocator requires inline storage");

    static constexpr size_t InlineStackBytes = DefaultStackBytes;

    explicit StackGrowAllocator(size_t stackBytes = InlineStackBytes)
        : StackCapacity(sanitizeStackBytes(stackBytes)) {}

    StackGrowAllocator(const StackGrowAllocator &) = delete;
    StackGrowAllocator &operator=(const StackGrowAllocator &) = delete;

    void *allocate_bytes(size_t size, size_t alignment = alignof(std::max_align_t)) override {
        std::byte *candidate = StackStorage + StackOffset;
        size_t     space = StackCapacity - StackOffset;
        void      *aligned = candidate;

        if (std::align(alignment, size, aligned, space)) {
            auto *bytePtr = static_cast<std::byte *>(aligned);
            StackOffset = static_cast<size_t>(bytePtr - StackStorage) + size;
            return bytePtr;
        }

        return BasicAllocator::allocateRaw(size, alignment);
    }

    void deallocate_bytes(void *ptr, size_t size,
                          size_t alignment = alignof(std::max_align_t)) override {
        (void)alignment;
        if (!ptr)
            return;

        std::byte *bytePtr = static_cast<std::byte *>(ptr);
        std::byte *begin = StackStorage;
        std::byte *end = begin + StackCapacity;

        if (bytePtr >= begin && bytePtr < end) {
            size_t allocStart = static_cast<size_t>(bytePtr - begin);
            size_t allocEnd = allocStart + size;
            if (allocEnd == StackOffset)
                StackOffset = allocStart;
            return;
        }

        BasicAllocator::deallocateRaw(ptr, size, alignment);
    }

    void reset() override { StackOffset = 0; }

    size_t stackBytesUsed() const { return StackOffset; }
    size_t stackBytesCapacity() const { return StackCapacity; }

  private:
    static constexpr size_t sanitizeStackBytes(size_t value) {
        if (value == 0 || value > InlineStackBytes)
            return InlineStackBytes;
        return value;
    }

    alignas(AlignSize) std::byte StackStorage[InlineStackBytes];
    size_t StackCapacity = InlineStackBytes;
    size_t StackOffset = 0;
};

StackGrowAllocator() -> StackGrowAllocator<>;
StackGrowAllocator(size_t) -> StackGrowAllocator<>;

} // namespace scc

#endif // SCC_ALLOCATOR_STACK_GROW_ALLOCATOR_H
