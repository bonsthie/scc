#ifndef SCC_ALLOCATOR_ALLOCATOR_H
#define SCC_ALLOCATOR_ALLOCATOR_H

#include <cstddef>
#include <cstring>
#include <string_view>
#include <utility>

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
