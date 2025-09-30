#ifndef SCC_PREPROCESSOR_DEFINE_H
#define SCC_PREPROCESSOR_DEFINE_H

#include "scc/Token/TokenStream.h"
#include <stack>

namespace scc {

class DefineTokenStack : public TokenStream {
	std::stack<Token> TokenStack;

  public:
    DefineTokenStack(std::stack<Token> &&TokenStack) : TokenStack(std::move(TokenStack)) {}

    Token next() {
		Token &next = TokenStack.top();
		TokenStack.pop();
		return std::move(next);
	}
};
} // namespace scc

#endif // SCC_PREPROCESSOR_DEFINE_H
