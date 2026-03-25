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
    explicit TypedAllocator(Allocator &alloc) : AllocPtr(&alloc) {}
    explicit TypedAllocator(Allocator *alloc) {
        if (alloc)
            AllocPtr = alloc;
    }

    void resetAllocator(Allocator &alloc) { AllocPtr = &alloc; }

    void *allocate_bytes(size_t size, size_t alignment = alignof(std::max_align_t)) {
        return allocator().allocate_bytes(size, alignment);
    }

    void deallocate_bytes(void *ptr, size_t size, size_t alignment = alignof(std::max_align_t)) {
        allocator().deallocate_bytes(ptr, size, alignment);
    }

    T *allocate(size_t n = 1) {
        return static_cast<T *>(allocate_bytes(sizeof(T) * n, alignof(T)));
    }

    void deallocate(T *ptr, size_t n = 1) {
        deallocate_bytes(ptr, sizeof(T) * n, alignof(T));
    }

  private:
    Allocator &allocator() { return *AllocPtr; }
    const Allocator &allocator() const { return *AllocPtr; }
};

}

#endif // SCC_ALLOCATOR_TYPEDALLOCATOR
