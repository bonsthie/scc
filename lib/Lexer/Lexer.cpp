#include "Lexer.h"

Lexer::Lexer(std::string_view &str) : _str(str) {}

Token Lexer::nextToken() {

    Token tok;
    do {
        tok = nextTokenRaw();
    } while (tok.is(tok::space) || tok.is(tok::comment) || tok.is(tok::eol));
    _lastToken = tok;
    return tok;
}

Token Lexer::nextTokenRaw() {
    _nextRawToken =  _nextTokenRaw();
	return _nextRawToken;
}

Token Lexer::_nextTokenRaw() {
    // if the token is already consume by peak();
    if (_nextToken.is(tok::not_init) == false) {
        Token t = _nextToken;
        _nextToken.type = tok::not_init;
        return t;
    }

    int last_char = _getChar();

    std::string tok;
    if (last_char == '\0') {
        return Token(tok::eof);
    } else if (isspace(last_char)) {
        return _handleSpaceToken(last_char);
    } else if (isalnum(last_char)) {
        return _handleNumToken(last_char);
    } else if (isalpha(last_char) || last_char == '_'){
        return _handleKeyword(last_char);
    } 

    return Token(tok::unknown, std::string(1, last_char));
}

Token Lexer::peak() {
    if (_nextToken.is(tok::not_init)) {
        _nextToken = nextToken();
    }
    return _nextToken;
}

int Lexer::_getChar(void) {
    int c =  _str[_position];
	_position++;
	return c;
}
