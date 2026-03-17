#ifndef SCC_LEX_SIZEDCHAR
#define SCC_LEX_SIZEDCHAR

#include <cstdint>

// Logical character read by the lexer.
// `consumed` is the number of raw source characters used to produce it.
// Example: "defi\\\nne" -> reading after 'i' returns {'n', 3}.
struct SizedChar {
    int     value;
    uint8_t size;

    bool operator==(SizedChar &sc) { return value == sc.value; }
    bool operator!=(SizedChar &sc) { return value != sc.value; }
    bool operator>=(SizedChar &sc) { return value >= sc.value; }
    bool operator<=(SizedChar &sc) { return value <= sc.value; }
    bool operator<(SizedChar &sc) { return value < sc.value; }
    bool operator>(SizedChar &sc) { return value > sc.value; }

    bool operator==(int c) { return value == c; }
    bool operator!=(int c) { return value != c; }
    bool operator>=(int c) { return value >= c; }
    bool operator<=(int c) { return value <= c; }
    bool operator<(int c) { return value < c; }
    bool operator>(int c) { return value > c; }
};

#endif // SCC_LEX_SIZEDCHAR
