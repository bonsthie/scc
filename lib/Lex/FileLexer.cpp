
#include "scc/Lex/FileLexer.h"
#include "scc/FileManager/File.h"
#include "scc/FileManager/MemoryBufferView.h"
#include "scc/Token/Token.h"
#include <cassert>
#include <cctype>
#include <iostream>

using namespace scc;

bool FileLexer::next(Token &CurTok) {
    do {
        // return true if an erro append while parsing a token
        // manly for comment eof error
        if (nextRaw(CurTok))
            return true;
    } while (CurTok.is(tok::space, tok::comment_line, tok::comment, tok::eol));
    LastToken = CurTok;
    if (LastToken.is(tok::eof))
        return true;
    return false;
}

bool FileLexer::nextRaw(Token &CurTok) {
    // if the token is already consume by peak();
    if (NextToken.is(tok::not_init) == false) {
        Token t = NextToken;
        NextToken.setTokenKind(tok::not_init);
        return false;
    }

    CurTok.setPosBegin(Pos);
    int LastChar = getChar();
    CurTok.setFileID(&FID);
    bool eof;

    if (LastChar == '\0') {
        CurTok.setTokenKind(tok::eof);
        eof = true;
    } else if (isspace(LastChar)) {
        eof = handleSpaceToken(CurTok, LastChar);
    } else if (isdigit(LastChar) || (LastChar == '.' && isdigit(peakChar()))) {
        eof = handleNumToken(CurTok, LastChar);
    } else if (isalpha(LastChar) || LastChar == '_') {
        eof = handleKeyword(CurTok, LastChar);
    } else {
        eof = LexSign(CurTok, LastChar);
    }

    CurTok.setPosEnd(Pos);
    return eof;
}

bool FileLexer::lexInclude(Token &CurTok) {
    bool err;

    while (1) {

        if (nextRaw(CurTok))
            return true;

		// this works for some reason but not with comment line
		// # include /*
		// */ "foo.h"
        if (CurTok.is(tok::space, tok::comment))
            continue;

        if (CurTok.is(tok::string_literal)) // #include "foo.h"
            return false;

        if (CurTok.is(tok::comment_line)) { // #include // comment_line
            CurTok.setTokenKind(tok::eol);
            return false;
        }

        if (CurTok.is(tok::less)) { // #include <foo.h>
            std::string Str(1, '<');

            CurTok.setPosBegin(Pos);
            bool err = consumeCharUntil('>', Str);
            CurTok.setValue(std::move(Str));
            CurTok.setTokenKind(err == false ? tok::system_string : tok::unknown);
            CurTok.setPosEnd(Pos);
            return err;
        }

        CurTok.setTokenKind(tok::unknown);
        return true;
    }
}

int FileLexer::peakChar(void) {
    if (Pos.Buff <= MemBufferView.size())
        return MemBufferView[Pos.Buff];
    return 0;
}

int FileLexer::peakChar(int Idx) {
    if (Pos.Buff + Idx <= MemBufferView.size())
        return MemBufferView[Pos.Buff + Idx - 1];
    return 0;
}

int FileLexer::getChar(void) {
    int c = peakChar();
    consumeChar();
    return c;
}

void FileLexer::consumeChar(void) {
    if (MemBufferView[Pos.Buff] == '\n') {
        Pos.Column = 1;
        Pos.Line++;
    } else
        Pos.Column++;
    Pos.Buff++;
}

bool FileLexer::consumeCharUntil(int c) {
    int nextC;

    do {
        nextC = getChar();
    } while (nextC != c && nextC != 0);
    return nextC == 0;
}

bool FileLexer::consumeCharUntil(int c, std::string &Str) {
    int nextC;

    do {
        nextC = getChar();
        Str += nextC;
    } while (nextC != c && nextC != 0);
    return nextC == 0;
}

bool FileLexer::ConsumeCharIfEqual(int c) {
    int tmpChar = peakChar();
    if (tmpChar != c)
        return false;
    consumeChar();
    return true;
}

bool FileLexer::handleSpaceToken(Token &CurTok, int LastChar) {
    if (LastChar == '\n') {
        CurTok.setTokenKind(tok::eol);
        return 0;
    }

    int c = 0;
    while (1) {

        c = peakChar();
        if (!isspace(c) || c == '\n')
            break;
        consumeChar();
    }
    CurTok.setTokenKind(tok::space);
    return false;
}

// for now only basic number before c14 are handle
// what's work :
// '123'
// '123.'
// '123crampte+ee' work becuse there is  e before the '+'
// what's don't work :
// 100_000_000 dosen't work etc....
bool FileLexer::handleNumToken(Token &CurTok, int LastChar) {
    assert((isdigit(LastChar) || LastChar == '.') &&
           "Fist char of a number must be a digit or a dot");
    CurTok.setTokenKind(tok::numeric_constant);

    std::string Num;
    Num += LastChar;

    int eof = false;
    while (1) {
        LastChar = peakChar();
        if (LastChar == 0) {
            eof = true;
            break;
        }

        if (!(isdigit(LastChar) || isalnum(LastChar) || LastChar == '.' || LastChar == '+' ||
              LastChar == '-' || LastChar == '_'))
            break;
        if ((LastChar == '+' || LastChar == '-') && !(Num.back() != 'e' || Num.back() != 'E'))
            break;

        consumeChar();
        Num += LastChar;
    }
    CurTok.setValue(std::move(Num));
    return eof;
}

bool FileLexer::handleKeyword(Token &CurTok, int LastChar) {
    assert((isalnum(LastChar) || LastChar == '_') &&
           "Fist char of a keword must be a letter or an underscore");

    int         CurrentChar = 0;
    std::string identifyer;

    identifyer += LastChar;
    while (1) {
        CurrentChar = peakChar();

        if (!(isalnum(CurrentChar) || CurrentChar == '_'))
            break;
        identifyer += CurrentChar;
        consumeChar();
    }
    create_keyword_token(CurTok, std::move(identifyer));
    return false;
}

bool FileLexer::handleString(Token &CurTok, int Limiter) {
    if (Limiter == '\'')
        CurTok.setTokenKind(tok::char_constant);
    else
        CurTok.setTokenKind(tok::string_literal);

    std::string string;
    string += Limiter;

    int LastChar = 0;
    do {
        LastChar = getChar();

        string += LastChar;
        if (LastChar == '\\') {
            string += getChar();
        }

    } while (LastChar != Limiter && LastChar != '\n' && LastChar != 0);
    CurTok.setValue(std::move(string));
    if (LastChar == Limiter)
        return false;
    return true;
}

inline size_t FileLexer::LexSign(Token &CurTok, int LastChar) {
    switch (LastChar) {
    case '\n':
        CurTok.setTokenKind(tok::eol);
        return false;
    case '(':
        CurTok.setTokenKind(tok::l_paren);
        return false;
    case ')':
        CurTok.setTokenKind(tok::r_paren);
        return false;
    case '{':
        CurTok.setTokenKind(tok::l_brace);
        return false;
    case '}':
        CurTok.setTokenKind(tok::r_brace);
        return false;
    case '[':
        CurTok.setTokenKind(tok::l_square);
        return false;
    case ']':
        CurTok.setTokenKind(tok::r_square);
        return false;
    case ';':
        CurTok.setTokenKind(tok::semi);
        return false;
    case ',':
        CurTok.setTokenKind(tok::comma);
        return false;
    case ':':
        CurTok.setTokenKind(tok::colon);
        return false;
    case '?':
        CurTok.setTokenKind(tok::question);
        return false;
    case '~':
        CurTok.setTokenKind(tok::tilde);
        return false;

    case '.': {
        // Ellipsis: '...'
        if (peakChar() == '.' && peakChar(2) == '.') {
            consumeChar();
            consumeChar();
            CurTok.setTokenKind(tok::ellipsis);
            return false;
        }
        CurTok.setTokenKind(tok::dot);
        return false;
    }

    case '-': {
        // Prefer '->' first
        if (ConsumeCharIfEqual('>')) {
            CurTok.setTokenKind(tok::arrow);
            return false;
        }
        // Then '--' or '-='
        int t = getCharIfOneOf('-', '=');
        switch (t) {
        case '-':
            CurTok.setTokenKind(tok::minus_minus);
            return false;
        case '=':
            CurTok.setTokenKind(tok::minus_equal);
            return false;
        default:
            CurTok.setTokenKind(tok::minus);
            return false;
        }
    }

    case '+': {
        int t = getCharIfOneOf('+', '=');
        switch (t) {
        case '+':
            CurTok.setTokenKind(tok::plus_plus);
            return false;
        case '=':
            CurTok.setTokenKind(tok::plus_equal);
            return false;
        default:
            CurTok.setTokenKind(tok::plus);
            return false;
        }
    }

    case '*': {
        if (ConsumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::star_equal);
            return false;
        }
        CurTok.setTokenKind(tok::star);
        return false;
    }

    case '/': {
        int t = getCharIfOneOf('=', '/', '*');
        switch (t) {
        case '=':
            CurTok.setTokenKind(tok::slash_equal);
            return false;
        case '/':
            CurTok.setTokenKind(tok::comment_line);
            return consumeCharUntil('\n');

        case '*':
            CurTok.setTokenKind(tok::comment);
            while (consumeCharUntil('*') == false) {
                if (peakChar() == '/') {
                    consumeChar();
                    return false;
                }
            }
            return true;
        default:
            CurTok.setTokenKind(tok::slash);
            return false;
        }
    }

    case '%': {
        if (ConsumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::percent_equal);
            return false;
        }
        CurTok.setTokenKind(tok::percent);
        return false;
    }

    case '^': {
        if (ConsumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::caret_equal);
            return false;
        }
        CurTok.setTokenKind(tok::caret);
        return false;
    }

    case '|': {
        if (ConsumeCharIfEqual('|')) {
            CurTok.setTokenKind(tok::pipe_pipe);
            return false;
        }
        if (ConsumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::pipe_equal);
            return false;
        }
        CurTok.setTokenKind(tok::pipe);
        return false;
    }

    case '&': {
        if (ConsumeCharIfEqual('&')) {
            CurTok.setTokenKind(tok::amp_amp);
            return false;
        }
        if (ConsumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::amp_equal);
            return false;
        }
        CurTok.setTokenKind(tok::amp);
        return false;
    }

    case '!': {
        if (ConsumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::exclaim_equal);
            return false;
        }
        CurTok.setTokenKind(tok::exclaim);
        return false;
    }

    case '=': {
        if (ConsumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::equal_equal);
            return false;
        }
        CurTok.setTokenKind(tok::equal);
        return false;
    }

    case '<': {
        if (ConsumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::less_equal);
            return false;
        }
        CurTok.setTokenKind(tok::less);
        return false;
    }

    case '>': {
        if (ConsumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::greater_equal);
            return false;
        }
        CurTok.setTokenKind(tok::greater);
        return false;
    }

    case '#': {
        if (ConsumeCharIfEqual('#')) {
            CurTok.setTokenKind(tok::pp_hash_hash);
            return false;
        }
        CurTok.setTokenKind(tok::pp_hash);
        return false;
    }
    case '\'':
    case '"': {
        return handleString(CurTok, LastChar);
    }

    default:
        break;
    }

    CurTok.setTokenKind(tok::unknown);
    CurTok.setValue(std::string(1, static_cast<char>(LastChar)));
    // Default fallthrough — nothing matched here.
    return true;
}
