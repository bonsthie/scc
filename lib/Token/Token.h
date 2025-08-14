#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class tok {
#define TOK(x) x,
#define PP_TOKENKIND(id, string) id,
#include "TokenKinds.def"
};

struct Token {
    Token() : type(tok::not_init) {};
    Token(tok token) : type(token) {};
    Token(tok tokType, std::string &&tokenStr) : type(tokType), token(tokenStr) {}

    bool is(tok identifyer) { return type == identifyer; }

    tok        type;
    std::string token;
};

#endif // TOKEN_H
