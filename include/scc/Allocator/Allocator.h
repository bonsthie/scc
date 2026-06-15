#ifndef SCC_ALLOCATOR_ALLOCATOR_H
#define SCC_ALLOCATOR_ALLOCATOR_H

#include <cstddef>
#include <cstring>
#include <memory>
#include <string_view>
#include <type_traits>
#include <utility>

#include "scc/ADT/Span.h"

namespace scc {

class Allocator {

  public:
    virtual ~Allocator() = default;

    virtual void *allocateBytes(size_t Size, size_t Alignment = alignof(std::max_align_t)) = 0;
    virtual void deallocateBytes(void *Ptr, size_t Size,
                                  size_t Alignment = alignof(std::max_align_t)) {
        (void)Ptr;
        (void)Size;
        (void)Alignment;
    }

    virtual void reset() {}

    template <typename T>
    T *allocateArray(size_t Count = 1) {
        return static_cast<T *>(allocateBytes(sizeof(T) * Count, alignof(T)));
    }

    template <typename T>
    T *copyArray(const T *Data, size_t Count) {
        if (Count == 0)
            return nullptr;

        if constexpr (std::is_trivially_copyable_v<T>)
            return static_cast<T *>(copyBytes(Data, Count * sizeof(T), alignof(T)));

        T *NewData = allocateArray<T>(Count);
        std::uninitialized_copy(Data, Data + Count, NewData);
        return NewData;
    }

    template <typename T, size_t N>
    Span<T> toOwnedList(const Span<T, N> &Values) {
        return {copyArray(Values.data(), Values.size()), Values.size()};
    }

    template <typename T, typename... Args>
    T *construct(Args &&...VArgs) {
        T *Ptr = allocateArray<T>(1);
        return new (Ptr) T(std::forward<Args>(VArgs)...);
    }

    void *copyBytes(const void *Data, size_t Size, size_t Alignment = alignof(std::max_align_t)) {
        void *NewData = allocateBytes(Size, Alignment);
        return std::memcpy(NewData, Data, Size);
    }

    std::string_view allocString(std::string_view Str) {
        char *Buf = static_cast<char *>(copyBytes(Str.data(), Str.size(), alignof(char)));
        return {Buf, Str.size()};
    }
};

} // namespace scc

#endif // SCC_ALLOCATOR_ALLOCATOR_H
