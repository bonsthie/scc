#ifndef SCC_ALLOCATOR_TYPEDALLOCATOR
#define SCC_ALLOCATOR_TYPEDALLOCATOR

#include "scc/Allocator/Allocator.h"
#include "scc/Allocator/BasicAllocator.h"
namespace scc {

template <typename T>
class TypedAllocator {
    BasicAllocator DefaultAlloc;
    Allocator     *AllocPtr = &DefaultAlloc;

public:
    TypedAllocator() = default;
    explicit TypedAllocator(Allocator &Alloc) : AllocPtr(&Alloc) {}
    explicit TypedAllocator(Allocator *Alloc) {
        if (Alloc)
            AllocPtr = Alloc;
    }

    void resetAllocator(Allocator &Alloc) { AllocPtr = &Alloc; }

    void *allocateBytes(size_t Size, size_t Alignment = alignof(std::max_align_t)) {
        return allocator().allocateBytes(Size, Alignment);
    }

    void deallocateBytes(void *Ptr, size_t Size, size_t Alignment = alignof(std::max_align_t)) {
        allocator().deallocateBytes(Ptr, Size, Alignment);
    }

    T *allocate(size_t N = 1) {
        return static_cast<T *>(allocateBytes(sizeof(T) * N, alignof(T)));
    }

    void deallocate(T *Ptr, size_t N = 1) {
        deallocateBytes(Ptr, sizeof(T) * N, alignof(T));
    }

  private:
    Allocator &allocator() { return *AllocPtr; }
    const Allocator &allocator() const { return *AllocPtr; }
};

}

#endif // SCC_ALLOCATOR_TYPEDALLOCATOR
