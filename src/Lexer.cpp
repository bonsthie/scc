#include "Lexer.h"

Lexer::Lexer(std::string_view &str) : _str(str) {}

Token Lexer::next() {
    Token t = _next();
    _lastToken = t;
    return t;
}

Token Lexer::_next() {
    // if the token is already consume by peak();
    if (_nextToken.type != Token::Type::tok_not_init) {
        Token t = _nextToken;
        _nextToken.type = Token::Type::tok_not_init;
        return t;
    }

    int last_char = _str[_position];

    while (isspace(last_char)) {
		last_char = _getChar();
    }
}

Token Lexer::peak() {
    if (_nextToken.type != Token::Type::tok_not_init) {
        _nextToken = _next();
    }
    return _nextToken;
}

int Lexer::_getChar(void) {
    _position++;
    return _str[_position];
}
