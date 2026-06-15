#ifndef SCC_STRING_STRINGINTERNER_H
#define SCC_STRING_STRINGINTERNER_H

#include "scc/Allocator/BumpAllocator.h"
#include <cstddef>
#include <cstring>
#include <string_view>
#include <unordered_set>

namespace scc {

class StringInterner {
    BumpAllocator &Arena;

    std::unordered_set<std::string_view> Pool;

  public:
    StringInterner(BumpAllocator &Arena) : Arena(Arena) {}

    std::string_view intern(std::string_view Str) {
        if (Str.empty())
            return {};

        auto It = Pool.find(Str);
        if (It != Pool.end()) {
            return *It;
        }

        std::string_view InternedStr = Arena.allocString(Str);

        Pool.insert(InternedStr);

        return InternedStr;
    }
};

} // namespace scc

#endif // SCC_STRING_STRINGINTERNER_H
