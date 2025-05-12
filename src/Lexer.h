#ifndef LEXER_H
#define LEXER_H

#include "FileManager.h"
#include "Token.h"
#include <cstddef>
#include <string>

class Lexer {
  public:
    Lexer(std::string_view &str);

    Token nextToken();
    Token nextTokenRaw();

    Token peak();

  private:
    size_t            _position = 0;
    std::string_view &_str;

    Token _lastToken;
    Token _nextToken;

    Token _lastRawToken;
    Token _nextRawToken;

    // internal of the next function
    Token _nextTokenRaw();
    int   _getChar(void);
};

#endif /* LEXER_H */
