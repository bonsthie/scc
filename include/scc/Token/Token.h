#ifndef SCCLEXTOKEN_H
#define SCCLEXTOKEN_H

#include "scc/FileManager/FileID.h"
#include <string>
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
    std::string    Value;
    FileID        *FID{nullptr};

  public:
    explicit Token(tok::TokenKind TKind = tok::not_init) : TKind(TKind) {}
    explicit Token(tok::TokenKind TKind, std::string Value) : TKind(TKind), Value(Value) {}

    tok::TokenKind     getTokenKind() const { return TKind; }
    const std::string &getValue() const { return Value; }

    void setTokenKind(tok::TokenKind T) { TKind = T; };
    void setValue(std::string &&Word) { Value = std::move(Word); };
    void setFileID(FileID *ID) { FID = ID; }

    bool is(tok::TokenKind Tok) { return TKind == Tok; }

    template <typename T, typename... Args> bool is(T Tok, Args... TokTypes) {
        return is(Tok) || is(TokTypes...);
    }

    struct Pos {
        unsigned Buff = 0;
        unsigned Line = 1;
        unsigned Column = 1;

        Pos() = default;
        Pos(unsigned Line, unsigned Column) : Line(Line), Column(Column) {}
        Pos(const Pos &P) { *this = P; }
        Pos &operator=(const Pos &P) = default;
    };

    struct PosView {
        Pos     P;
        const FileID *FID{nullptr};

        void print(std::ostream &OS) const;
    };

  private:
    Pos PosBegin;
    Pos PosEnd;

  public:
    void       setPosEnd(const Pos &P) { PosEnd = P; }
    void       setPosBegin(const Pos &P) { PosBegin = P; }
    const Pos &getPosEnd() const { return PosEnd; }
    const Pos &getPosBegin() const { return PosBegin; }

    PosView posViewBegin() const { return {PosBegin, FID}; }
    PosView posViewEnd() const { return {PosEnd, FID}; }

    bool isStartOfLine() const { return PosBegin.Column == 1; }

    void print(std::ostream &OS) const;

    void flush() {
        TKind = tok::not_init;
        Value = "";
        FID = nullptr;
        PosEnd = Pos();
        PosBegin = Pos();
    }
};

// use a hash map of all the word is a Keyword and setup the CurTok
void create_keyword_token(Token &CurTok, std::string &&Word);

std::string stringify_token_kind(tok::TokenKind Kind);

// ostream utils
std::ostream &operator<<(std::ostream &OS, const Token &T);

std::ostream &operator<<(std::ostream &OS, const Token::PosView &V);

} // namespace scc

#endif // SCCLEXTOKEN_H
