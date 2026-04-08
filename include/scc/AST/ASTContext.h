#ifndef SCC_AST_ASTCONTEXT
#define SCC_AST_ASTCONTEXT

#include <cstddef>
#include <string_view>
#include <utility>

#include "scc/Allocator/Allocator.h"
#include "scc/Allocator/BumpAllocator.h"

namespace scc {

class ASTContext {
    BumpAllocator Alloc;

  public:
    ASTContext() = default;
    explicit ASTContext(size_t ChunkSize) : Alloc(ChunkSize) {}

    Allocator       &allocator() { return Alloc; }
    const Allocator &allocator() const { return Alloc; }

    void reset() { allocator().reset(); }

    template <typename T, size_t N>
    Span<T> toOwnedList(const Span<T, N> &Values) {
        return allocator().toOwnedList(Values);
    }

    template <typename T, typename... Args>
    T *construct(Args &&...ArgsList) {
        return allocator().construct<T>(std::forward<Args>(ArgsList)...);
    }
};

} // namespace scc

#endif // SCC_AST_ASTCONTEXT
