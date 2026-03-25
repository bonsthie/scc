#ifndef SCC_ALLOCATOR_STACK_GROW_ALLOCATOR_H
#define SCC_ALLOCATOR_STACK_GROW_ALLOCATOR_H

#include <cstddef>
#include <memory>

#include "scc/Allocator/Allocator.h"
#include "scc/Allocator/BasicAllocator.h"

namespace scc {

class StackGrowAllocator : public Allocator {
  public:
    static constexpr size_t DefaultStackBytes = 4096;

    explicit StackGrowAllocator(size_t stackBytes = DefaultStackBytes);

    StackGrowAllocator(const StackGrowAllocator &) = delete;
    StackGrowAllocator &operator=(const StackGrowAllocator &) = delete;

    void *allocate_bytes(size_t size, size_t alignment = alignof(std::max_align_t)) override;
    void deallocate_bytes(void *ptr, size_t size,
                          size_t alignment = alignof(std::max_align_t)) override;
    void reset() override;

    size_t stackBytesUsed() const { return StackOffset; }
    size_t stackBytesCapacity() const { return StackCapacity; }

  private:
    std::unique_ptr<std::byte[]> StackStorage;
    size_t                       StackCapacity = DefaultStackBytes;
    size_t                       StackOffset = 0;
};

} // namespace scc

#endif // SCC_ALLOCATOR_STACK_GROW_ALLOCATOR_H
