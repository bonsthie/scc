#include "scc/Allocator/StackGrowAllocator.h"

#include <algorithm>

namespace scc {

namespace {
constexpr size_t sanitizeStackBytes(size_t value) {
    return value == 0 ? StackGrowAllocator::DefaultStackBytes : value;
}
} // namespace

StackGrowAllocator::StackGrowAllocator(size_t stackBytes) {
    StackCapacity = sanitizeStackBytes(stackBytes);
    StackStorage.reset(new std::byte[StackCapacity]);
}

void *StackGrowAllocator::allocate_bytes(size_t size, size_t alignment) {
    std::byte *candidate = StackStorage.get() + StackOffset;
    size_t     space = StackCapacity - StackOffset;
    void      *aligned = candidate;

    if (std::align(alignment, size, aligned, space)) {
        auto *bytePtr = static_cast<std::byte *>(aligned);
        StackOffset = static_cast<size_t>(bytePtr - StackStorage.get()) + size;
        return bytePtr;
    }

    return BasicAllocator::allocateRaw(size, alignment);
}

void StackGrowAllocator::deallocate_bytes(void *ptr, size_t size, size_t alignment) {
    (void)alignment;
    if (!ptr)
        return;

    std::byte *bytePtr = static_cast<std::byte *>(ptr);
    std::byte *begin = StackStorage.get();
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

void StackGrowAllocator::reset() {
    StackOffset = 0;
}

} // namespace scc
