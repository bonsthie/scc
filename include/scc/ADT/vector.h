#ifndef SCC_ADT_VECTOR_H
#define SCC_ADT_VECTOR_H

#include "scc/ADT/BaseVector.h"
#include "scc/Allocator/BasicAllocator.h"
#include "scc/Allocator/StackGrowAllocator.h"
#include <cstddef>

namespace scc {

template <typename T, size_t StackSize>
using SmallVectorAllocator =
    StackGrowAllocator<(StackSize == 0 ? sizeof(T) : StackSize * sizeof(T)), alignof(T)>;

template <typename T, size_t StackSize = 16>
class SmallVector : public BaseVector<T, StackSize, SmallVectorAllocator<T, StackSize>> {
    using Base = BaseVector<T, StackSize, SmallVectorAllocator<T, StackSize>>;

  public:
    using Base::Base;
};

template <typename T, size_t StackSize = 16>
class vector : public BaseVector<T, StackSize, BasicAllocator> {
    using Base = BaseVector<T, StackSize, BasicAllocator>;

  public:
    using Base::Base;
};

} // namespace scc

#endif // SCC_ADT_VECTOR_H
