#ifndef SCCLEXTOKEN_H
#define SCCLEXTOKEN_H

#include <string>
namespace scc {

namespace tok {

enum TokenKind {
#define TOK(X) X,
#include "TokenKinds.def"

    NUMBER_OF_TOKENS
};

} // namespace tok

class Token {
    tok::TokenKind TK;
    std::string    Value;

  public:
    explicit Token(tok::TokenKind TK) : TK(TK) {}
    explicit Token(tok::TokenKind TK, std::string Value) : TK(TK), Value(Value) {}

    tok::TokenKind     getTokenKind() const { return TK; }
    const std::string &getValue() const { return Value; }
};

} // namespace scc

#endif // SCCLEXTOKEN_H
