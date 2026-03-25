#ifndef SCC_ALLOCATOR_TYPEDALLOCATOR
#define SCC_ALLOCATOR_TYPEDALLOCATOR

#include "scc/Allocator/Allocator.h"
namespace scc {

template <typename T>
class TypedAllocator {
    Allocator &A;

public:
    explicit TypedAllocator(Allocator& alloc) : A(alloc) {}

    T* allocate(size_t n) {
        return static_cast<T*>(
            A.allocate_bytes(sizeof(T) * n, alignof(T))
        );
    }

    void deallocate(T* ptr, size_t n) {
        A.deallocate_bytes(ptr, sizeof(T) * n, alignof(T));
    }
};

}

#endif // SCC_ALLOCATOR_TYPEDALLOCATOR
