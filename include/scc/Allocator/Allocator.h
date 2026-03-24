#ifndef SCC_ALLOCATOR_ALLOCATOR_H
#define SCC_ALLOCATOR_ALLOCATOR_H

#include <cstddef>
#include <cstring>
#include <string_view>
#include <utility>

namespace scc {

class Allocator {
    virtual void *rawAlloc(size_t size, size_t alignment = alignof(std::max_align_t)) = 0;
    virtual void  unalloc(void *ptr, size_t alignment = alignof(std::max_align_t)) {
        (void)ptr;
        (void)alignment;
    }

  public:
    virtual ~Allocator() = default;

    virtual void reset() {}

    template <typename T, typename... Args>
    T *alloc(Args &&...args) {
        void *ptr = rawAlloc(sizeof(T), alignof(T));
        return new (ptr) T(std::forward<Args>(args)...);
    }

    void *copy(const void *data, size_t size, size_t alignment = alignof(std::max_align_t)) {
        void *newData = rawAlloc(size, alignment);
        return std::memcpy(newData, data, size);
    }

    std::string_view allocString(std::string_view str,
                                 size_t           alignment = alignof(std::max_align_t)) {
        return {
            static_cast<const char *>(copy(str.data(), str.size(), alignment)), //
            str.size()                                                          //
        };
    }
};

} // namespace scc

#endif // SCC_ALLOCATOR_ALLOCATOR_H
