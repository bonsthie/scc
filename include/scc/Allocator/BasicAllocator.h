#ifndef SCC_ALLOCATOR_BASIC_ALLOCATOR_H
#define SCC_ALLOCATOR_BASIC_ALLOCATOR_H

#include <cstddef>
#include <new>

#include "scc/Allocator/Allocator.h"

namespace scc {

class BasicAllocator : public Allocator {
  public:
    void *allocate_bytes(size_t size, size_t alignment = alignof(std::max_align_t)) override {
        void *ptr = nullptr;
#if __cpp_aligned_new
        ptr = ::operator new(size, std::align_val_t(alignment));
#else
        ptr = ::operator new(size);
#endif
        return ptr;
    }

    void deallocate_bytes(void *ptr, size_t size, size_t alignment = alignof(std::max_align_t)) override {
        (void)size;
        if (!ptr)
            return;
#if __cpp_aligned_new
        ::operator delete(ptr, std::align_val_t(alignment));
#else
        ::operator delete(ptr);
#endif
    }
};

} // namespace scc

#endif // SCC_ALLOCATOR_BASIC_ALLOCATOR_H
