#ifndef SCC_ALLOCATOR_STACKGROWALLOCATOR_H
#define SCC_ALLOCATOR_STACKGROWALLOCATOR_H

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

    explicit StackGrowAllocator(size_t StackBytes = InlineStackBytes)
        : StackCapacity(sanitizeStackBytes(StackBytes)) {}

    StackGrowAllocator(const StackGrowAllocator &) = delete;
    StackGrowAllocator &operator=(const StackGrowAllocator &) = delete;

    void *allocateBytes(size_t Size, size_t Alignment = alignof(std::max_align_t)) override {
        std::byte *Candidate = StackStorage + StackOffset;
        size_t     Space = StackCapacity - StackOffset;
        void      *Aligned = Candidate;

        if (std::align(Alignment, Size, Aligned, Space)) {
            auto *BytePtr = static_cast<std::byte *>(Aligned);
            StackOffset = static_cast<size_t>(BytePtr - StackStorage) + Size;
            return BytePtr;
        }

        return BasicAllocator::allocateRaw(Size, Alignment);
    }

    void deallocateBytes(void *Ptr, size_t Size,
                          size_t Alignment = alignof(std::max_align_t)) override {
        (void)Alignment;
        if (!Ptr)
            return;

        std::byte *BytePtr = static_cast<std::byte *>(Ptr);
        std::byte *Begin = StackStorage;
        std::byte *End = Begin + StackCapacity;

        if (BytePtr >= Begin && BytePtr < End) {
            size_t AllocStart = static_cast<size_t>(BytePtr - Begin);
            size_t AllocEnd = AllocStart + Size;
            if (AllocEnd == StackOffset)
                StackOffset = AllocStart;
            return;
        }

        BasicAllocator::deallocateRaw(Ptr, Size, Alignment);
    }

    void reset() override { StackOffset = 0; }

    size_t stackBytesUsed() const { return StackOffset; }
    size_t stackBytesCapacity() const { return StackCapacity; }

  private:
    static constexpr size_t sanitizeStackBytes(size_t Value) {
        if (Value == 0 || Value > InlineStackBytes)
            return InlineStackBytes;
        return Value;
    }

    alignas(AlignSize) std::byte StackStorage[InlineStackBytes];
    size_t StackCapacity = InlineStackBytes;
    size_t StackOffset = 0;
};

StackGrowAllocator() -> StackGrowAllocator<>;
StackGrowAllocator(size_t) -> StackGrowAllocator<>;

} // namespace scc

#endif // SCC_ALLOCATOR_STACKGROWALLOCATOR_H
