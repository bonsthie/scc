#ifndef SCCLEXTOKEN_H
#define SCCLEXTOKEN_H

#include "scc/FileManager/FileID.h"
#include "scc/FileManager/MemoryBufferView.h"
#include <string>
#include <string_view>
namespace scc {

namespace tok {

enum TokenKind {
#define TOK(X) X,
#include "TokenKinds.def"

    NUMBER_OF_TOKENS
};

} // namespace tok

class Token {
    tok::TokenKind TKind;

    // Value is always clean
    std::string_view Value;      // TODO remove for a ptr and size and a view() func
    std::string_view DirtyValue; // TODO remove and find a better way to do that

    bool        IsDirty = false;
    SourceRange Range;

  public:
    explicit Token(tok::TokenKind TKind = tok::not_init) : TKind(TKind) {}
    explicit Token(tok::TokenKind TKind, std::string_view Value) : TKind(TKind) { setValue(Value); }

    tok::TokenKind getTokenKind() const { return TKind; }
    // Borrowed lexeme view; copy via toOwnedValue() if you need to keep it.
    const std::string_view &getValue() const { return Value; }
    const std::string_view &getDirtyValue() const { return DirtyValue; }

    void setTokenKind(tok::TokenKind T) { TKind = T; };
    void setValue(std::string_view Word) { Value = Word; };
    void setFileID(FileID *ID) { Range.FID = ID; }

    void setDirty(bool value) { IsDirty = value; }
    bool isDirty() const { return IsDirty; }
    void setDirtyValue(std::string_view Word) { DirtyValue = Word; };

    bool is(tok::TokenKind Tok) { return TKind == Tok; }

    template <typename T, typename... Args>
    bool is(T Tok, Args... TokTypes) {
        return is(Tok) || is(TokTypes...);
    }

  public:
    void setPosBegin(const MemoryViewPos &P) { Range.Begin = P; }
    void setPosEnd(const MemoryViewPos &P) { Range.End = P; }
    void setRange(const SourceRange &R) { Range = R; }

    const MemoryViewPos &getPosBegin() const { return Range.Begin; }
    const MemoryViewPos &getPosEnd() const { return Range.End; }
    const SourceRange   &getRange() const { return Range; }

    PosView posViewBegin() const { return Range.posViewBegin(); }
    PosView posViewEnd() const { return Range.posViewEnd(); }

    bool isStartOfLine() const { return Range.Begin.Column == 1; }

    void print(std::ostream &OS) const;

    void flush() {
        TKind = tok::not_init;
        Value = std::string_view();
        DirtyValue = std::string_view();
        Range = SourceRange();
        IsDirty = false;
    }
};

// use a hash map of all the word is a Keyword and setup the CurTok
void create_keyword_token(Token &CurTok);

std::string clean_token(std::string_view str);

std::string stringify_token_kind(tok::TokenKind Kind);

// ostream utils
std::ostream &operator<<(std::ostream &OS, const Token &T);

} // namespace scc

#endif // SCCLEXTOKEN_H
