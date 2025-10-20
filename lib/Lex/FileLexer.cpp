
#include "scc/Lex/FileLexer.h"
#include "scc/FileManager/File.h"
#include "scc/FileManager/MemoryBufferView.h"
#include "scc/Token/Token.h"
#include <cctype>

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

    int LastChar = getChar();

    if (LastChar == '\0') {
        CurTok.setTokenKind(tok::eof);
        return true;
    } else if (isspace(LastChar)) {
        return handleSpaceToken(CurTok, LastChar);
    } else if (isdigit(LastChar)) {
        return handleNumToken(CurTok, LastChar);
    } else if (isalpha(LastChar) || LastChar == '_') {
        return handleKeyword(CurTok, LastChar);
    }

    return LexSign(CurTok, LastChar);
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

void FileLexer::consumeChar(void) {
    if (MemBufferView[BuffPos] == '\n') {
        ColumnPos = 0;
        LinePos++;
    } else
        ColumnPos++;
    BuffPos++;
}

bool FileLexer::consumeCharUntil(int c) {
    int nextC;

    do {
        nextC = getChar();
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
    int c = 0;
    while (1) {
        c = peakChar();
        if (!isspace(c))
            break;
        consumeChar();
    }
    CurTok.setTokenKind(tok::space);
    return false;
}

// ([0-9]*[.])?[0-9]+([eE][-+]?\d+)?
bool FileLexer::handleNumToken(Token &CurTok, int LastChar) {
	CurTok.setTokenKind(tok::numeric_constant);
	std::string Num;

	int tmpChar = peakChar();
	while (isdigit(tmpChar)) {
		Num += tmpChar;
		consumeChar();
	}
	// need to do a preparsing of the number even if we consume garbedge just because wee need to see if e+ in float

}

bool FileLexer::handleKeyword(Token &CurTok, int LastChar) {

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

    case '.': {
        // Ellipsis: '...'
        if (ConsumeCharIfEqual('.') && ConsumeCharIfEqual('.')) {
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

    default:
        break;
    }

    CurTok.setTokenKind(tok::unknown);
    CurTok.setValue(std::string(1, static_cast<char>(LastChar)));
    // Default fallthrough — nothing matched here.
    return true;
}
