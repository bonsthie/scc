#ifndef SCC_LIB_LEX_TRIGRAPH_H
#define SCC_LIB_LEX_TRIGRAPH_H


namespace scc {

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
