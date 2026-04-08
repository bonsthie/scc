#ifndef SCC_ALLOCATOR_ALLOCATOR_H
#define SCC_ALLOCATOR_ALLOCATOR_H

#include <memory>
#include <cstddef>
#include <cstring>
#include <string_view>
#include <type_traits>
#include <utility>

#include "scc/ADT/Span.h"

namespace scc {

class Allocator {

  public:
    virtual ~Allocator() = default;

    virtual void *allocate_bytes(size_t size, size_t alignment = alignof(std::max_align_t)) = 0;
    virtual void deallocate_bytes(void *ptr, size_t size,
                                  size_t alignment = alignof(std::max_align_t)) {
        (void)ptr;
        (void)size;
        (void)alignment;
    }

    virtual void reset() {}

    template <typename T>
    T *allocateArray(size_t count = 1) {
        return static_cast<T *>(allocate_bytes(sizeof(T) * count, alignof(T)));
    }

    template <typename T>
    T *copyArray(const T *data, size_t count) {
        if (count == 0)
            return nullptr;

        if constexpr (std::is_trivially_copyable_v<T>)
            return static_cast<T *>(copyBytes(data, count * sizeof(T), alignof(T)));

        T *newData = allocateArray<T>(count);
        std::uninitialized_copy(data, data + count, newData);
        return newData;
    }

    template <typename T, size_t N>
    Span<T> toOwnedList(const Span<T, N> &values) {
        return {copyArray(values.data(), values.size()), values.size()};
    }

    template <typename T, typename... Args>
    T *construct(Args &&...args) {
        T *ptr = allocateArray<T>(1);
        return new (ptr) T(std::forward<Args>(args)...);
    }

    void *copyBytes(const void *data, size_t size, size_t alignment = alignof(std::max_align_t)) {
        void *newData = allocate_bytes(size, alignment);
        return std::memcpy(newData, data, size);
    }

    std::string_view allocString(std::string_view str) {
        char *buf = static_cast<char *>(copyBytes(str.data(), str.size(), alignof(char)));
        return {buf, str.size()};
    }
};

} // namespace scc

#endif // SCC_ALLOCATOR_ALLOCATOR_H
