#include "scc/Lex/FileLexer.h"
#include "scc/FileManager/MemoryBufferView.h"
#include "scc/Token/Token.h"

using namespace scc;

bool FileLexer::next(Token &CurTok) {
    do {
    } while (CurTok.is(tok::space, tok::comment, tok::eol));
    LastToken = CurTok;
    return false;
}

bool FileLexer::nextRaw(Token &CurTok) {
    // if the token is already consume by peak();
    if (NextToken.is(tok::not_init) == false) {
        Token t = NextToken;
        NextToken.setTokenKind(tok::not_init);
        return false;
    }

    int LastChar = getChar();

    if (LastChar == '\0') {
        CurTok.setTokenKind(tok::eof);
        return true;
    } else if (isspace(LastChar)) {
        return handleSpaceToken(CurTok, LastChar);
    } else if (isalnum(LastChar)) {
        return handleNumToken(CurTok, LastChar);
    } else if (isalpha(LastChar) || LastChar == '_') {
        return handleKeyword(CurTok, LastChar);
    }
    // todo sign

    CurTok.setTokenKind(tok::unknown);
    return true;
}

// Token FileLexer::peak() {
//     if (NextToken.is(tok::not_init)) {
//         NextToken = nextToken();
//     }
//     return NextToken;
// }

void FileLexer::consumeChar(void) {
    if (MemBufferView[BuffPos] == '\n') {
        ColumnPos = 0;
        LinePos++;
    } else
        ColumnPos++;
    BuffPos++;
}

int FileLexer::peakChar(void) { return MemBufferView[BuffPos]; }

int FileLexer::peakChar(int Idx) {
    if (BuffPos + Idx <= MemBufferView.size())
        return MemBufferView[BuffPos + Idx - 1];
    return -1;
}

int FileLexer::getChar(void) {
    int c = MemBufferView[BuffPos];
    consumeChar();
    return c;
}

bool FileLexer::handleSpaceToken(Token &CurTok, int LastChar) {
    //
    return false;
}

bool FileLexer::handleNumToken(Token &CurTok, int LastChar) {
    //
    return false;
}
bool FileLexer::handleKeyword(Token &CurTok, int LastChar) { return false; }

inline size_t FileLexer::LexSign(Token &CurTok, int LastChar) {
    switch (LastChar) {
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

    case '.':
        if (peakChar() == '.' && peakChar(2) == '.') {
            consumeChar();
            consumeChar();
            CurTok.setTokenKind(tok::ellipsis);
            return false;
        }
        CurTok.setTokenKind(tok::dot);
        return false;

    case '-':
        if (peakChar() == '>') {
            consumeChar();
            CurTok.setTokenKind(tok::arrow);
        }
        if (peakChar() == '-') {
            consumeChar();
            CurTok.setTokenKind(tok::minus_minus);
            return false;
        }
        if (peakChar() == '=') {
            consumeChar();
            CurTok.setTokenKind(tok::minus_equal);
            return false;
        }
        CurTok.setTokenKind(tok::minus);
        return false;

    case '+':
        if (peakChar() == '+') {
            consumeChar();
            CurTok.setTokenKind(tok::plus_plus);
            return false;
        }
        if (peakChar() == '=') {
            consumeChar();
            CurTok.setTokenKind(tok::plus_equal);
            return false;
        }
        CurTok.setTokenKind(tok::plus);
        return false;

    case '*':
        if (peakChar() == '=') {
            consumeChar();
            CurTok.setTokenKind(tok::star_equal);
            return false;
        }
        CurTok.setTokenKind(tok::star);
        return false;

    case '/':
        if (peakChar() == '=') {
            consumeChar();
            CurTok.setTokenKind(tok::slash_equal);
            return false;
        }
        CurTok.setTokenKind(tok::slash);
        return false;

    case '%':
        if (peakChar() == '=') {
            consumeChar();
            CurTok.setTokenKind(tok::percent_equal);
            return false;
        }
        CurTok.setTokenKind(tok::percent);
        return false;

    case '^':
        if (peakChar() == '=') {
            consumeChar();
            CurTok.setTokenKind(tok::caret_equal);
            return false;
        }
        CurTok.setTokenKind(tok::caret);
        return false;

    case '|':
        if (peakChar() == '|') {
            consumeChar();
            CurTok.setTokenKind(tok::pipe_pipe);
            return false;
        }
        consumeChar();
        if (peakChar() == '=') {
            CurTok.setTokenKind(tok::pipe_equal);
            return false;
        }
        CurTok.setTokenKind(tok::pipe);
        return false;

    case '&':
        if (peakChar() == '&') {
            consumeChar();
            CurTok.setTokenKind(tok::amp_amp);
            return false;
        }
        if (peakChar() == '=') {
            consumeChar();
            CurTok.setTokenKind(tok::amp_equal);
            return false;
        }
        CurTok.setTokenKind(tok::amp);
        return false;

    case '!':
        if (peakChar() == '=') {
            consumeChar();
            CurTok.setTokenKind(tok::exclaim_equal);
            return false;
        }
        CurTok.setTokenKind(tok::exclaim);
        return false;

    case '=':
        if (peakChar() == '=') {
            consumeChar();
            CurTok.setTokenKind(tok::equal_equal);
            return false;
        }
        CurTok.setTokenKind(tok::equal);
        return false;

    case '<':
        if (peakChar() == '=') {
            consumeChar();
            CurTok.setTokenKind(tok::less_equal);
            return false;
        }
        CurTok.setTokenKind(tok::less);
        return false;

    case '>':
        if (peakChar() == '=') {
            consumeChar();
            CurTok.setTokenKind(tok::greater_equal);
            return false;
        }
        CurTok.setTokenKind(tok::greater);
        return false;
    case '#': {
        if (peakChar() == '#') {
            consumeChar();
            CurTok.setTokenKind(tok::pp_hash_hash);
            return false;
        }
        CurTok.setTokenKind(tok::pp_hash);
        return false;
    }

    default:
        break;
    }
    CurTok.setTokenKind(tok::unknown);
    return true;
}
