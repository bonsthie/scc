#include "scc/Lex/DecodeChar.h"

namespace scc {
namespace {

// Trigraph Rules TODO:
// - Part of the C standard in: C89, C90, C99, C11, C17
// - Removed in: C23
//
// Compiler behavior (Clang/GCC-like):
// - Disabled by default in GNU modes (gnu*)
// - Enabled in strict modes (e.g. -std=c89, sometimes others depending on compiler)
// - Can be explicitly enabled with -ftrigraphs or clang driver style : -Wtrigraphs
// Maybe do Digraphs for c99+
int handle_trigraph(int c) {
	char decoded = 0;

	switch (c) {
		case '=': decoded = '#'; break;
		case '/': decoded = '\\'; break;
		case '\'': decoded = '^'; break;
		case '(': decoded = '['; break;
		case ')': decoded = ']'; break;
		case '!': decoded = '|'; break;
		case '<': decoded = '{'; break;
		case '>': decoded = '}'; break;
		case '-': decoded = '~'; break;
	}

    return decoded;
}

} // namespace

// Returns the next logical character, and the number of PHYSICAL bytes it took.
SizedChar decode_logical_char(const char *Ptr, const char *End) {
    if (Ptr >= End)
        return {0, 0};

    int c = *Ptr;
    uint8_t consumed = 1;

    // Fast path for 99% of characters
    if (c != '\\' && c != '?')
        return {c, consumed};

    if (c == '?' && Ptr + 2 < End && Ptr[1] == '?') {
        int trigraph_value = handle_trigraph(Ptr[2]);
        if (trigraph_value) {
            c = trigraph_value;
            consumed = 3;
        }
    }

    const char *nextPtr = Ptr + consumed;

    // 2. Line splicing
    if (c == '\\') {
        // Unix LF
        if (nextPtr < End && *nextPtr == '\n') {
            SizedChar next = decode_logical_char(nextPtr + 1, End);
            next.size += consumed + 1;
            return next;
        }
        // Windows CRLF
        if (nextPtr + 1 < End && nextPtr[0] == '\r' && nextPtr[1] == '\n') {
            SizedChar next = decode_logical_char(nextPtr + 2, End);
            next.size += consumed + 2;
            return next;
        }
    }

    return {c, consumed};
}

} // namespace scc
