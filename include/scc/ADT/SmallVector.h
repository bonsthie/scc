#ifndef SCC_ADT_SMALLVECTOR_H
#define SCC_ADT_SMALLVECTOR_H

#include "scc/ADT/BaseVector.h"
#include "scc/Allocator/BasicAllocator.h"
#include "scc/Allocator/StackGrowAllocator.h"
#include <cstddef>

namespace scc {

template <typename T, size_t StackSize = 16, typename Alloc = StackGrowAllocator>
class SmallVector : public BaseVector<T, StackSize, Alloc> {
    using Base = BaseVector<T, StackSize, Alloc>;

  public:
    using Base::Base;
};

template <typename T, size_t StackSize = 16, typename Alloc = BasicAllocator>
class vector : public BaseVector<T, StackSize, Alloc> {
    using Base = BaseVector<T, StackSize, Alloc>;

  public:
    using Base::Base;
};

} // namespace scc

#endif // SCC_ADT_SMALLVECTOR_H
