
#include "scc/Lex/FileLexer.h"
#include "scc/FileManager/MemoryBufferView.h"
#include "scc/Lex/SizedChar.h"
#include "scc/Token/Token.h"
#include <cassert>
#include <cctype>

using namespace scc;

namespace {

MemoryViewPos advance_pos(const char *Begin, const char *Ptr, MemoryViewPos Pos) {
    while (Begin < Ptr) {
        if (*Begin == '\n') {
            Pos.Line++;
            Pos.Column = 1;
        } else {
            Pos.Column++;
        }
        Pos.Buff++;
        Begin++;
    }
    return Pos;
}

int handle_trigraph(int c) {
    char decoded = 0;

    switch (c) {
    case '=':
        decoded = '#';
        break;
    case '/':
        decoded = '\\';
        break;
    case '\'':
        decoded = '^';
        break;
    case '(':
        decoded = '[';
        break;
    case ')':
        decoded = ']';
        break;
    case '!':
        decoded = '|';
        break;
    case '<':
        decoded = '{';
        break;
    case '>':
        decoded = '}';
        break;
    case '-':
        decoded = '~';
        break;
    }

    return decoded;
}

} // namespace

const LangOpt &FileLexer::defaultLangOpt() {
    static const LangOpt Opts{};
    return Opts;
}

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
        CurTok = NextToken;
        NextToken.setTokenKind(tok::not_init);
        return false;
    }

    CurTok.setPosBegin(Pos);
    SizedChar LastChar = getChar();
    CurTok.setFileID(&FID);
    bool eof;

    if (LastChar == '\0') {
        CurTok.setTokenKind(tok::eof);
        eof = true;
    } else if (isspace(LastChar.value)) {
        eof = handleSpaceToken(CurTok, LastChar);
    } else if (isdigit(LastChar.value) || (LastChar == '.' && isdigit(peakChar().value))) {
        eof = handleNumToken(CurTok, LastChar);
    } else if (isalpha(LastChar.value) || LastChar == '_') {
        eof = handleKeyword(CurTok, LastChar);
    } else {
        eof = LexSign(CurTok, LastChar);
    }

    CurTok.setPosEnd(Pos);
    ParseDirtyToken = false;
    return eof;
}

bool FileLexer::lexInclude(Token &CurTok) {
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
            CurTok.setTokenKind(err == false ? tok::system_string : tok::unknown);
            CurTok.setPosEnd(Pos);
            setTokenValue(CurTok);

            if (err) {
                EM.report(err::error)
                    .at(CurTok.posViewEnd())
                    .msg("include missing closing ")
                    .Char('>');
                CurTok.flush();
            }
            return err;
        }

        CurTok.setTokenKind(tok::unknown);
        return false;
    }
}

SizedChar FileLexer::peakCharAtIdx(int Idx) {
    const char *Ptr = MemBufferView.raw() + Pos.Buff + Idx;
    return decodeLogicalChar(Ptr);
}

SizedChar FileLexer::decodeLogicalChar(const char *Ptr) {
    const char *End = MemBufferView.end();
    if (Ptr >= End)
        return {0, 0};

    int     c = *Ptr;
    uint8_t consumed = 1;

    if (c != '\\' && c != '?')
        return {c, consumed};

    if (c == '?' && Ptr + 2 < End && Ptr[1] == '?') {
        int trigraphValue = handle_trigraph(Ptr[2]);
        if (trigraphValue) {
            if (!Opts.TrigraphEnable) {
                const char    *Begin = MemBufferView.raw() + Pos.Buff;
                MemoryViewPos  DiagPos = advance_pos(Begin, Ptr, Pos);
                EM.report(err::warning)
                    .at({DiagPos, &FID})
                    .msg("trigraph ignored [-Wtrigraphs]");
                return {c, consumed};
            }
            c = trigraphValue;
            consumed = 3;
        }
    }

    const char *NextPtr = Ptr + consumed;

    if (c == '\\') {
        if (NextPtr < End && *NextPtr == '\n') {
            SizedChar Next = decodeLogicalChar(NextPtr + 1);
            Next.size += consumed + 1;
            return Next;
        }

        if (NextPtr + 1 < End && NextPtr[0] == '\r' && NextPtr[1] == '\n') {
            SizedChar Next = decodeLogicalChar(NextPtr + 2);
            Next.size += consumed + 2;
            return Next;
        }
    }

    return {c, consumed};
}

SizedChar FileLexer::peakChar(int Idx) {
    int       offset = 0;
    SizedChar sc;

    for (int i = 0; i < Idx; i++) {
        sc = peakCharAtIdx(offset);
        offset += sc.size + 1;
    }
    return sc;
}

SizedChar FileLexer::getChar(void) {
    SizedChar c = peakChar();
    consumeChar(c);
    return c;
}

void FileLexer::consumeChar() {
    if (MemBufferView[Pos.Buff] == '\n') {
        Pos.Column = 1;
        Pos.Line++;
    } else
        Pos.Column++;
    Pos.Buff++;
}

void FileLexer::consumeChar(SizedChar sc) {
    if (sc.size > 1)
        ParseDirtyToken = true;
    for (int i = 0; i < sc.size; i++)
        consumeChar();
}

void FileLexer::consumeChar(int size) {
    for (int i = 0; i < size; i++)
        consumeChar();
}

bool FileLexer::consumeCharUntil(int c) {
    SizedChar nextC;

    do {
        nextC = getChar();
    } while (nextC != c && nextC != 0);
    return nextC == 0;
}

bool FileLexer::consumeCharUntil(int c, std::string &Str) {
    SizedChar nextC;

    do {
        nextC = getChar();
        Str += nextC.value;
    } while (nextC != c && nextC != 0 && nextC != '\n');
    return nextC != c;
}

bool FileLexer::ConsumeCharIfEqual(int c) {
    SizedChar tmpChar = peakChar();
    if (tmpChar != c)
        return false;
    consumeChar();
    return true;
}

std::string_view FileLexer::makeStringView(const MemoryViewPos &Begin,
                                           const MemoryViewPos &End) const {
    return MemBufferView.getStringView(Begin, End);
}

void FileLexer::setTokenValue(Token &CurTok, const MemoryViewPos &End) {
    if (ParseDirtyToken) {
        std::string_view baseString = makeStringView(CurTok.getPosBegin(), End);
        std::string      clean = clean_token(baseString);
        CurTok.setValue(SI.intern(clean));
        CurTok.setDirtyValue(baseString);
        CurTok.setDirty(true);
    } else {
        std::string_view V = SI.intern(makeStringView(CurTok.getPosBegin(), End));
        CurTok.setValue(V);
    }
}

void FileLexer::setTokenValue(Token &CurTok) { setTokenValue(CurTok, CurTok.getPosEnd()); }

bool FileLexer::handleSpaceToken(Token &CurTok, SizedChar LastChar) {
    if (LastChar == '\n') {
        CurTok.setTokenKind(tok::eol);
        return 0;
    }

    SizedChar sc;
    while (1) {

        sc = peakChar();
        if (!isspace(sc.value) || sc.value == '\n')
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
bool FileLexer::handleNumToken(Token &CurTok, SizedChar LastChar) {
    assert((isdigit(LastChar.value) || LastChar == '.') &&
           "Fist char of a number must be a digit or a dot");
    CurTok.setTokenKind(tok::numeric_constant);
    SizedChar NextChar;

    int eof = false;
    while (1) {
        NextChar = peakChar();
        if (NextChar == 0) {
            eof = true;
            break;
        }

        if (!(isdigit(NextChar.value) || isalnum(NextChar.value) || NextChar == '.' ||
              NextChar == '+' || NextChar == '-' || NextChar == '_'))
            break;
        if ((NextChar == '+' || NextChar == '-') && !(LastChar != 'e' || LastChar != 'E'))
            break;

        consumeChar(NextChar);
        LastChar = NextChar;
    }
    setTokenValue(CurTok, Pos);
    return eof;
}

bool FileLexer::handleKeyword(Token &CurTok, SizedChar LastChar) {
    assert((isalnum(LastChar.value) || LastChar == '_') &&
           "Fist char of a keword must be a letter or an underscore");

    SizedChar CurrentChar;

    while (1) {
        CurrentChar = peakChar();

        if (!(isalnum(CurrentChar.value) || CurrentChar == '_'))
            break;
        consumeChar(CurrentChar);
    }

    setTokenValue(CurTok, Pos);
    create_keyword_token(CurTok);
    return false;
}

bool FileLexer::handleString(Token &CurTok, SizedChar Limiter) {
    if (Limiter == '\'')
        CurTok.setTokenKind(tok::char_constant);
    else
        CurTok.setTokenKind(tok::string_literal);

    SizedChar LastChar;
    do {
        LastChar = getChar();

        if (LastChar == '\\') {
            getChar();
        }

    } while (LastChar != Limiter && LastChar != '\n' && LastChar != 0);
    if (LastChar == Limiter) {
        setTokenValue(CurTok, Pos);
        return false;
    }

    EM.report(err::error) //
        .at(CurTok.posViewEnd())
        .msg("missing closing ")
        .Char(Limiter.value);
    CurTok.flush();
    return true;
}

inline size_t FileLexer::LexSign(Token &CurTok, SizedChar LastChar) {
    auto setAndReturn = [&](tok::TokenKind Kind) {
        CurTok.setTokenKind(Kind);
        setTokenValue(CurTok, Pos);
        return false;
    };

    switch (LastChar.value) {
    case '\n':
        return setAndReturn(tok::eol);
    case '(':
        return setAndReturn(tok::l_paren);
    case ')':
        return setAndReturn(tok::r_paren);
    case '{':
        return setAndReturn(tok::l_brace);
    case '}':
        return setAndReturn(tok::r_brace);
    case '[':
        return setAndReturn(tok::l_square);
    case ']':
        return setAndReturn(tok::r_square);
    case ';':
        return setAndReturn(tok::semi);
    case ',':
        return setAndReturn(tok::comma);
    case ':':
        return setAndReturn(tok::colon);
    case '?':
        return setAndReturn(tok::question);
    case '~':
        return setAndReturn(tok::tilde);

    case '.': {
        // Ellipsis: '...'
        if (consumeIfIs('.', '.') == true) {
            CurTok.setTokenKind(tok::ellipsis);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return setAndReturn(tok::dot);
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
            setTokenValue(CurTok, Pos);
            return false;
        case '=':
            CurTok.setTokenKind(tok::minus_equal);
            setTokenValue(CurTok, Pos);
            return false;
        default:
            return setAndReturn(tok::minus);
        }
    }

    case '+': {
        int t = getCharIfOneOf('+', '=');
        switch (t) {
        case '+':
            CurTok.setTokenKind(tok::plus_plus);
            setTokenValue(CurTok, Pos);
            return false;
        case '=':
            CurTok.setTokenKind(tok::plus_equal);
            setTokenValue(CurTok, Pos);
            return false;
        default:
            return setAndReturn(tok::plus);
        }
    }

    case '*': {
        if (ConsumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::star_equal);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return setAndReturn(tok::star);
    }

    case '/': {
        int t = getCharIfOneOf('=', '/', '*');
        switch (t) {
        case '=':
            CurTok.setTokenKind(tok::slash_equal);
            setTokenValue(CurTok, Pos);
            return false;
        case '/':
            CurTok.setTokenKind(tok::comment_line);
            return consumeCharUntil('\n');

        case '*':
            CurTok.setTokenKind(tok::comment);
            while (consumeCharUntil('*') == false) {
                if (consumeIfIs('/') == true) {
                    return false;
                }
            }

            EM.report(err::error) //
                .at(CurTok.posViewBegin())
                .msg("unterminated /* comment");
            return true;
        default:
            return setAndReturn(tok::slash);
        }
    }

    case '%': {
        if (ConsumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::percent_equal);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return setAndReturn(tok::percent);
    }

    case '^': {
        if (ConsumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::caret_equal);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return setAndReturn(tok::caret);
    }

    case '|': {
        if (ConsumeCharIfEqual('|')) {
            CurTok.setTokenKind(tok::pipe_pipe);
            setTokenValue(CurTok, Pos);
            return false;
        }
        if (ConsumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::pipe_equal);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return setAndReturn(tok::pipe);
    }

    case '&': {
        if (ConsumeCharIfEqual('&')) {
            CurTok.setTokenKind(tok::amp_amp);
            setTokenValue(CurTok, Pos);
            return false;
        }
        if (ConsumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::amp_equal);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return setAndReturn(tok::amp);
    }

    case '!': {
        if (ConsumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::exclaim_equal);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return setAndReturn(tok::exclaim);
    }

    case '=': {
        if (ConsumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::equal_equal);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return setAndReturn(tok::equal);
    }

    case '<': {
        if (ConsumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::less_equal);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return setAndReturn(tok::less);
    }

    case '>': {
        if (ConsumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::greater_equal);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return setAndReturn(tok::greater);
    }

    case '#': {
        if (ConsumeCharIfEqual('#')) {
            CurTok.setTokenKind(tok::pp_hash_hash);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return setAndReturn(tok::pp_hash);
    }
    case '\'':
    case '"': {
        return handleString(CurTok, LastChar);
    }

    default:
        break;
    }

    CurTok.setTokenKind(tok::unknown);
    setTokenValue(CurTok, Pos);
    // Default fallthrough — nothing matched here.
    return true;
}
