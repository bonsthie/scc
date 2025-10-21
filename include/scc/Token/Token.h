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
    FileID        *FID;

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
        unsigned Line = 0;
        unsigned Column = 0;

        Pos() = default;
        Pos(Pos &P) { *this = P; }

        Pos &operator=(const Pos &P) {
            Buff = P.Buff;
            Line = P.Line;
            Column = P.Column;

            return *this;
        };
    };

  private:
    Pos PosBegin;
    Pos PosEnd;

  public:
    void       setPosEnd(const Pos &P) { PosEnd = P; }
    void       setPosBegin(const Pos &P) { PosBegin = P; }
    const Pos &getPosEnd() const { return PosEnd; }
    const Pos &getPosBegin() const { return PosBegin; }

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

} // namespace scc

#endif // SCCLEXTOKEN_H
