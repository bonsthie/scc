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
    tok::TokenKind TKind;
    std::string    Value;

  public:
    explicit Token(tok::TokenKind TKind = tok::not_init) : TKind(TKind) {}
    explicit Token(tok::TokenKind TKind, std::string Value) : TKind(TKind), Value(Value) {}

    tok::TokenKind     getTokenKind() const { return TKind; }
    const std::string &getValue() const { return Value; }

    void setTokenKind(tok::TokenKind T) { TKind = T; };
    void setValue(std::string &&Word) { Value = std::move(Word); };

    bool is(tok::TokenKind Tok) { return TKind == Tok; }

    template <typename T, typename... Args> bool is(T Tok, Args... TokTypes) {
        return is(Tok) || is(TokTypes...);
    }
};

// use a hash map of all the word is a Keyword and setup the CurTok
void create_keyword_token(Token &CurTok, std::string &&Word);

std::string stringify_token_kind(tok::TokenKind Kind);

} // namespace scc

#endif // SCCLEXTOKEN_H
