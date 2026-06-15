#ifndef SCC_STRING_STRINGSWITCH_H
#define SCC_STRING_STRINGSWITCH_H

#include <optional>
#include <string_view>

namespace scc {

template <typename T>
class StringSwitch {
    std::string_view S;
    std::optional<T> Res;

  public:
    explicit StringSwitch(std::string_view s) : S(s) {}

    StringSwitch &caseValue(std::string_view Lit, const T &v) {
        if (!Res && S == Lit)
            Res = v;
        return *this;
    }

    T defaultValue(const T &v) const { return Res.value_or(v); }

    operator T() const { return *Res; }
};

} // namespace scc

#endif // SCC_STRING_STRINGSWITCH_H
