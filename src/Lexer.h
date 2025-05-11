#ifndef LEXER_H
#define LEXER_H

#include "FileManager.h"
#include <cstddef>
#include <string>

struct Token {
    enum class Type {
        tok_not_init = -1,
        tok_eof = -2,

        // commands
        tok_def = -3,
        tok_extern = -4,

        // primary
        tok_identifier = -5,
        tok_number = -6,
    };

    Token() : type(Type::tok_not_init){};
    Token(Type tokType, std::string &&tokenStr) : type(tokType), token(tokenStr) {}

    Type        type;
    std::string token;
};

class Lexer {
  public:
    Lexer(std::string_view &str);

    Token next();
    Token peak();

  private:
    size_t            _position = 0;
    std::string_view &_str;

    Token _lastToken;
    Token _nextToken;

    // internal of the next function
    Token _next();
    int  _getChar(void);
};

#endif /* LEXER_H */
