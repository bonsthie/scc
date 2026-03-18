#ifndef SCC_LIB_LEX_TRIGRAPH_H
#define SCC_LIB_LEX_TRIGRAPH_H


namespace scc {

// Trigraph Rules TODO:
// - Part of the C standard in: C89, C90, C99, C11, C17
// - Removed in: C23
//
// Compiler behavior (Clang/GCC-like):
// - Disabled by default in GNU modes (gnu*)
// - Enabled in strict modes (e.g. -std=c89, sometimes others depending on compiler)
// - Can be explicitly enabled with -ftrigraphs
// Maybe do Digraphs for c99+

inline int handle_trigraph(int c) {
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

}

#endif // SCC_LIB_LEX_TRIGRAPH_H
