#ifndef SCC_ALLOCATOR_BASICALLOCATOR_H
#define SCC_ALLOCATOR_BASICALLOCATOR_H

#include <cstddef>
#include <new>

#include "scc/Allocator/Allocator.h"

namespace scc {

class BasicAllocator : public Allocator {
  public:
    static void *allocateRaw(size_t Size, size_t Alignment = alignof(std::max_align_t)) {
        void *Ptr = nullptr;
#if __cpp_aligned_new
        Ptr = ::operator new(Size, std::align_val_t(Alignment));
#else
        ptr = ::operator new(size);
#endif
        return Ptr;
    }

    static void deallocateRaw(void *Ptr, size_t Size, size_t Alignment = alignof(std::max_align_t)) {
        (void)Size;
        if (!Ptr)
            return;
#if __cpp_aligned_new
        ::operator delete(Ptr, std::align_val_t(Alignment));
#else
        ::operator delete(ptr);
#endif
    }

    void *allocateBytes(size_t Size, size_t Alignment = alignof(std::max_align_t)) override {
        return allocateRaw(Size, Alignment);
    }

    void deallocateBytes(void *Ptr, size_t Size, size_t Alignment = alignof(std::max_align_t)) override {
        deallocateRaw(Ptr, Size, Alignment);
    }
};

} // namespace scc

#endif // SCC_ALLOCATOR_BASICALLOCATOR_H
