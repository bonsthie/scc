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
    FileID          *FID{nullptr};

    bool IsDirty = false;

  public:
    explicit Token(tok::TokenKind TKind = tok::not_init) : TKind(TKind) {}
    explicit Token(tok::TokenKind TKind, std::string_view Value) : TKind(TKind) { setValue(Value); }

    tok::TokenKind getTokenKind() const { return TKind; }
    // Borrowed lexeme view; copy via toOwnedValue() if you need to keep it.
    const std::string_view &getValue() const { return Value; }
    const std::string_view &getDirtyValue() const { return DirtyValue; }

    void setTokenKind(tok::TokenKind T) { TKind = T; };
    void setValue(std::string_view Word) { Value = Word; };
    void setFileID(FileID *ID) { FID = ID; }

    void setDirty(bool value) { IsDirty = value; }
    bool isDirty() const { return IsDirty; }
    void setDirtyValue(std::string_view Word) { DirtyValue = Word; };

    bool is(tok::TokenKind Tok) { return TKind == Tok; }

    template <typename T, typename... Args> bool is(T Tok, Args... TokTypes) {
        return is(Tok) || is(TokTypes...);
    }

    struct PosView {
        MemoryViewPos P;
        const FileID *FID{nullptr};

        void print(std::ostream &OS) const;
    };

  private:
    MemoryViewPos PosBegin;
    MemoryViewPos PosEnd;

  public:
    void                 setPosEnd(const MemoryViewPos &P) { PosEnd = P; }
    void                 setPosBegin(const MemoryViewPos &P) { PosBegin = P; }
    const MemoryViewPos &getPosEnd() const { return PosEnd; }
    const MemoryViewPos &getPosBegin() const { return PosBegin; }

    PosView posViewBegin() const { return {PosBegin, FID}; }
    PosView posViewEnd() const { return {PosEnd, FID}; }

    bool isStartOfLine() const { return PosBegin.Column == 1; }

    void print(std::ostream &OS) const;

    void flush() {
        TKind = tok::not_init;
        Value = std::string_view();
        DirtyValue = std::string_view();
        FID = nullptr;
        PosEnd = MemoryViewPos();
        PosBegin = MemoryViewPos();
        IsDirty = false;
    }
};

// use a hash map of all the word is a Keyword and setup the CurTok
void create_keyword_token(Token &CurTok);

std::string clean_token(std::string_view str);

std::string stringify_token_kind(tok::TokenKind Kind);

// ostream utils
std::ostream &operator<<(std::ostream &OS, const Token &T);

std::ostream &operator<<(std::ostream &OS, const Token::PosView &V);

} // namespace scc

#endif // SCCLEXTOKEN_H
