#ifndef SCC_AST_QUALIFIERS_H
#define SCC_AST_QUALIFIERS_H

#include <cstdint>

#include "scc/Support/Attributes.h"

namespace scc {

struct Qualifiers {
    SCC_PREFERRED_TYPE(bool)
    uint64_t IsConst : 1 = 0;

    SCC_PREFERRED_TYPE(bool)
    uint64_t IsRestrict : 1 = 0;

    SCC_PREFERRED_TYPE(bool)
    uint64_t IsVolatile : 1 = 0;

    constexpr void merge(const Qualifiers &Other) {
        IsConst |= Other.IsConst;
        IsRestrict |= Other.IsRestrict;
        IsVolatile |= Other.IsVolatile;
    }

    constexpr bool operator==(const Qualifiers &Other) const {
        return IsConst == Other.IsConst && IsRestrict == Other.IsRestrict &&
               IsVolatile == Other.IsVolatile;
    }

    constexpr bool operator!=(const Qualifiers &Other) const { return !(*this == Other); }
};

} // namespace scc

#endif // SCC_AST_QUALIFIERS_H
