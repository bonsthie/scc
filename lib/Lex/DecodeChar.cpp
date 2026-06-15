#include "scc/Lex/DecodeChar.h"

namespace scc {

// Trigraph Rules TODO:
// - Part of the C standard in: C89, C90, C99, C11, C17
// - Removed in: C23
//
// Compiler behavior (Clang/GCC-like):
// - Disabled by default in GNU modes (gnu*)
// - Enabled in strict modes (e.g. -std=c89, sometimes others depending on compiler)
// - Can be explicitly enabled with -ftrigraphs or clang driver style : -Wtrigraphs
// Maybe do Digraphs for c99+
static int handle_trigraph(int c) {
    char Decoded = 0;

    switch (c) {
    case '=':
        Decoded = '#';
        break;
    case '/':
        Decoded = '\\';
        break;
    case '\'':
        Decoded = '^';
        break;
    case '(':
        Decoded = '[';
        break;
    case ')':
        Decoded = ']';
        break;
    case '!':
        Decoded = '|';
        break;
    case '<':
        Decoded = '{';
        break;
    case '>':
        Decoded = '}';
        break;
    case '-':
        Decoded = '~';
        break;
    }

    return Decoded;
}

// Returns the next logical character, and the number of PHYSICAL bytes it took.
SizedChar decode_logical_char(const char *Ptr, const char *End) {
    if (Ptr >= End)
        return {0, 0};

    int     c = *Ptr;
    uint8_t Consumed = 1;

    // Fast path for 99% of characters
    if (c != '\\' && c != '?')
        return {c, Consumed};

    if (c == '?' && Ptr + 2 < End && Ptr[1] == '?') {
        int TrigraphValue = handle_trigraph(Ptr[2]);
        if (TrigraphValue) {
            c = TrigraphValue;
            Consumed = 3;
        }
    }

    const char *NextPtr = Ptr + Consumed;

    // 2. Line splicing
    if (c == '\\') {
        // Unix LF
        if (NextPtr < End && *NextPtr == '\n') {
            SizedChar Next = decode_logical_char(NextPtr + 1, End);
            Next.Size += Consumed + 1;
            return Next;
        }
        // Windows CRLF
        if (NextPtr + 1 < End && NextPtr[0] == '\r' && NextPtr[1] == '\n') {
            SizedChar Next = decode_logical_char(NextPtr + 2, End);
            Next.Size += Consumed + 2;
            return Next;
        }
    }

    return {c, Consumed};
}

} // namespace scc
