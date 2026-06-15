#ifndef SCC_LEX_SIZEDCHAR_H
#define SCC_LEX_SIZEDCHAR_H

#include <cstdint>

namespace scc {

// Logical character read by the lexer.
// `consumed` is the number of raw source characters used to produce it.
// Example: "defi\\\nne" -> reading after 'i' returns {'n', 3}.
struct SizedChar {
    int     Value;
    uint8_t Size;

    bool operator==(SizedChar &Sc) { return Value == Sc.Value; }
    bool operator!=(SizedChar &Sc) { return Value != Sc.Value; }
    bool operator>=(SizedChar &Sc) { return Value >= Sc.Value; }
    bool operator<=(SizedChar &Sc) { return Value <= Sc.Value; }
    bool operator<(SizedChar &Sc) { return Value < Sc.Value; }
    bool operator>(SizedChar &Sc) { return Value > Sc.Value; }

    bool operator==(int c) { return Value == c; }
    bool operator!=(int c) { return Value != c; }
    bool operator>=(int c) { return Value >= c; }
    bool operator<=(int c) { return Value <= c; }
    bool operator<(int c) { return Value < c; }
    bool operator>(int c) { return Value > c; }
};

} // namespace scc

#endif // SCC_LEX_SIZEDCHAR_H
