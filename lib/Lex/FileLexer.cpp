
#include "scc/Lex/FileLexer.h"
#include "scc/FileManager/MemoryBufferView.h"
#include "scc/Lex/SizedChar.h"
#include "scc/Token/Token.h"
#include <cassert>
#include <cctype>

using namespace scc;

static MemoryViewPos advance_pos(const char *Begin, const char *Ptr, MemoryViewPos Pos) {
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

static int decode_trigraph_char(int c) {
    char Decoded = 0;

    switch (c) {
    case '=':
        Decoded = '#';
        break;
    case '/':
        Decoded = '\\';
        break;
    case '\'':
        Decoded = '^';
        break;
    case '(':
        Decoded = '[';
        break;
    case ')':
        Decoded = ']';
        break;
    case '!':
        Decoded = '|';
        break;
    case '<':
        Decoded = '{';
        break;
    case '>':
        Decoded = '}';
        break;
    case '-':
        Decoded = '~';
        break;
    }

    return Decoded;
}

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
    bool Eof;

    if (LastChar == '\0') {
        CurTok.setTokenKind(tok::eof);
        Eof = true;
    } else if (isspace(LastChar.Value)) {
        Eof = handleSpaceToken(CurTok, LastChar);
    } else if (isdigit(LastChar.Value) || (LastChar == '.' && isdigit(peakChar().Value))) {
        Eof = handleNumToken(CurTok, LastChar);
    } else if (isalpha(LastChar.Value) || LastChar == '_') {
        Eof = handleKeyword(CurTok, LastChar);
    } else {
        Eof = lexSign(CurTok, LastChar);
    }

    CurTok.setPosEnd(Pos);
    ParseDirtyToken = false;
    return Eof;
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
            bool Err = consumeCharUntil('>', Str);
            CurTok.setTokenKind(Err == false ? tok::system_string : tok::unknown);
            CurTok.setPosEnd(Pos);
            setTokenValue(CurTok);

            if (Err) {
                EM.report(err::error)
                    .at(CurTok.posViewEnd())
                    .msg("include missing closing ")
                    .quoteChar('>');
                CurTok.flush();
            }
            return Err;
        }

        CurTok.setTokenKind(tok::unknown);
        return false;
    }
}

SizedChar FileLexer::peakCharAtIdx(int Idx) {
    const char *Ptr = MemBufferView.raw() + Pos.Buff + Idx;
    return decodeLogicalChar(Ptr);
}

void FileLexer::reportTrigraphWarning(const char *Ptr, int TrigraphValue, bool Converted) {
    if (!Opts.trigraphsWarning)
        return;

    const char    *Begin = MemBufferView.raw() + Pos.Buff;
    MemoryViewPos  DiagPos = advance_pos(Begin, Ptr, Pos);
    Error         &Diag = EM.report(err::warning);
    Diag.at({DiagPos, &FID});

    if (Converted) {
        Diag.msg("trigraph converted to ")
            .quoteChar(static_cast<char>(TrigraphValue))
            .msg(" character [-Wtrigraphs]");
        return;
    }

    Diag.msg("trigraph ignored [-Wtrigraphs]");
}

SizedChar FileLexer::handleTrigraph(const char *Ptr, bool EmitWarning) {
    const char *End = MemBufferView.end();
    if (*Ptr != '?' || Ptr + 2 >= End || Ptr[1] != '?')
        return {'?', 1};

    int TrigraphValue = decode_trigraph_char(Ptr[2]);
    if (!TrigraphValue)
        return {'?', 1};

    if (!Opts.trigraphsEnable) {
        if (EmitWarning)
            reportTrigraphWarning(Ptr, TrigraphValue, false);
        return {'?', 1};
    }

    if (EmitWarning)
        reportTrigraphWarning(Ptr, TrigraphValue, true);

    return {TrigraphValue, 3};
}

SizedChar FileLexer::decodeLogicalChar(const char *Ptr, bool EmitTrigraphWarning) {
    const char *End = MemBufferView.end();
    if (Ptr >= End)
        return {0, 0};

    int     c = *Ptr;
    uint8_t Consumed = 1;

    if (c != '\\' && c != '?')
        return {c, Consumed};

    if (c == '?' && Ptr + 2 < End && Ptr[1] == '?') {
        SizedChar Trigraph = handleTrigraph(Ptr, EmitTrigraphWarning);
        c = Trigraph.Value;
        Consumed = Trigraph.Size;
    }

    const char *NextPtr = Ptr + Consumed;

    if (c == '\\') {
        if (NextPtr < End && *NextPtr == '\n') {
            SizedChar Next = decodeLogicalChar(NextPtr + 1, EmitTrigraphWarning);
            Next.Size += Consumed + 1;
            return Next;
        }

        if (NextPtr + 1 < End && NextPtr[0] == '\r' && NextPtr[1] == '\n') {
            SizedChar Next = decodeLogicalChar(NextPtr + 2, EmitTrigraphWarning);
            Next.Size += Consumed + 2;
            return Next;
        }
    }

    return {c, Consumed};
}

SizedChar FileLexer::peakChar(int Idx) {
    int       Offset = 0;
    SizedChar Sc;

    for (int i = 0; i < Idx; i++) {
        Sc = peakCharAtIdx(Offset);
        Offset += Sc.Size + 1;
    }
    return Sc;
}

SizedChar FileLexer::getChar(void) {
    const char *Ptr = MemBufferView.raw() + Pos.Buff;
    SizedChar   c = decodeLogicalChar(Ptr, true);
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

void FileLexer::consumeChar(SizedChar Sc) {
    if (Sc.Size > 1)
        ParseDirtyToken = true;
    for (int i = 0; i < Sc.Size; i++)
        consumeChar();
}

void FileLexer::consumeChar(int Size) {
    for (int i = 0; i < Size; i++)
        consumeChar();
}

bool FileLexer::consumeCharUntil(int c) {
    SizedChar NextC;

    do {
        NextC = getChar();
    } while (NextC != c && NextC != 0);
    return NextC == 0;
}

bool FileLexer::consumeCharUntil(int c, std::string &Str) {
    SizedChar NextC;

    do {
        NextC = getChar();
        Str += NextC.Value;
    } while (NextC != c && NextC != 0 && NextC != '\n');
    return NextC != c;
}

bool FileLexer::consumeCharIfEqual(int c) {
    SizedChar TmpChar = peakChar();
    if (TmpChar != c)
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
        std::string_view BaseString = makeStringView(CurTok.getPosBegin(), End);
        std::string      Clean = clean_token(BaseString);
        CurTok.setValue(SI.intern(Clean));
        CurTok.setDirtyValue(BaseString);
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

    SizedChar Sc;
    while (1) {

        Sc = peakChar();
        if (!isspace(Sc.Value) || Sc.Value == '\n')
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
    assert((isdigit(LastChar.Value) || LastChar == '.') &&
           "Fist char of a number must be a digit or a dot");
    CurTok.setTokenKind(tok::numeric_constant);
    SizedChar NextChar;

    int Eof = false;
    while (1) {
        NextChar = peakChar();
        if (NextChar == 0) {
            Eof = true;
            break;
        }

        if (!(isdigit(NextChar.Value) || isalnum(NextChar.Value) || NextChar == '.' ||
              NextChar == '+' || NextChar == '-' || NextChar == '_'))
            break;
        if ((NextChar == '+' || NextChar == '-') && !(LastChar != 'e' || LastChar != 'E'))
            break;

        consumeChar(NextChar);
        LastChar = NextChar;
    }
    setTokenValue(CurTok, Pos);
    return Eof;
}

bool FileLexer::handleKeyword(Token &CurTok, SizedChar LastChar) {
    assert((isalnum(LastChar.Value) || LastChar == '_') &&
           "Fist char of a keword must be a letter or an underscore");

    SizedChar CurrentChar;

    while (1) {
        CurrentChar = peakChar();

        if (!(isalnum(CurrentChar.Value) || CurrentChar == '_'))
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
        .quoteChar(Limiter.Value);
    CurTok.flush();
    return true;
}

inline size_t FileLexer::lexSign(Token &CurTok, SizedChar LastChar) {
    auto SetAndReturn = [&](tok::TokenKind Kind) {
        CurTok.setTokenKind(Kind);
        setTokenValue(CurTok, Pos);
        return false;
    };

    switch (LastChar.Value) {
    case '\n':
        return SetAndReturn(tok::eol);
    case '(':
        return SetAndReturn(tok::l_paren);
    case ')':
        return SetAndReturn(tok::r_paren);
    case '{':
        return SetAndReturn(tok::l_brace);
    case '}':
        return SetAndReturn(tok::r_brace);
    case '[':
        return SetAndReturn(tok::l_square);
    case ']':
        return SetAndReturn(tok::r_square);
    case ';':
        return SetAndReturn(tok::semi);
    case ',':
        return SetAndReturn(tok::comma);
    case ':':
        return SetAndReturn(tok::colon);
    case '?':
        return SetAndReturn(tok::question);
    case '~':
        return SetAndReturn(tok::tilde);

    case '.': {
        // Ellipsis: '...'
        if (consumeIfIs('.', '.') == true) {
            CurTok.setTokenKind(tok::ellipsis);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return SetAndReturn(tok::dot);
    }

    case '-': {
        // Prefer '->' first
        if (consumeCharIfEqual('>')) {
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
            return SetAndReturn(tok::minus);
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
            return SetAndReturn(tok::plus);
        }
    }

    case '*': {
        if (consumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::star_equal);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return SetAndReturn(tok::star);
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
            return SetAndReturn(tok::slash);
        }
    }

    case '%': {
        if (consumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::percent_equal);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return SetAndReturn(tok::percent);
    }

    case '^': {
        if (consumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::caret_equal);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return SetAndReturn(tok::caret);
    }

    case '|': {
        if (consumeCharIfEqual('|')) {
            CurTok.setTokenKind(tok::pipe_pipe);
            setTokenValue(CurTok, Pos);
            return false;
        }
        if (consumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::pipe_equal);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return SetAndReturn(tok::pipe);
    }

    case '&': {
        if (consumeCharIfEqual('&')) {
            CurTok.setTokenKind(tok::amp_amp);
            setTokenValue(CurTok, Pos);
            return false;
        }
        if (consumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::amp_equal);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return SetAndReturn(tok::amp);
    }

    case '!': {
        if (consumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::exclaim_equal);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return SetAndReturn(tok::exclaim);
    }

    case '=': {
        if (consumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::equal_equal);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return SetAndReturn(tok::equal);
    }

    case '<': {
        if (consumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::less_equal);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return SetAndReturn(tok::less);
    }

    case '>': {
        if (consumeCharIfEqual('=')) {
            CurTok.setTokenKind(tok::greater_equal);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return SetAndReturn(tok::greater);
    }

    case '#': {
        if (consumeCharIfEqual('#')) {
            CurTok.setTokenKind(tok::pp_hash_hash);
            setTokenValue(CurTok, Pos);
            return false;
        }
        return SetAndReturn(tok::pp_hash);
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
