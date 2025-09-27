#ifndef SCC_STRING_STRINGSWITCH_H
#define SCC_STRING_STRINGSWITCH_H

#include <optional>
#include <string_view>

namespace scc {
namespace str {

template <typename T> class StringSwitch {
    std::string_view s_;
    std::optional<T> res_;

  public:
    explicit StringSwitch(std::string_view s) : s_(s) {}

    StringSwitch &Case(std::string_view lit, const T &v) {
        if (!res_ && s_ == lit)
            res_ = v;
        return *this;
    }

    T Default(const T &v) const { return res_.value_or(v); }

    operator T() const { return *res_; }
};

} // namespace str

} // namespace scc
//

#endif // SCC_STRING_STRINGSWITCH_H
