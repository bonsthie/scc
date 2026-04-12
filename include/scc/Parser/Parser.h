#ifndef SCC_PARSER_PARSER_H
#define SCC_PARSER_PARSER_H

#include "ParseDeclarator.h"
#include "ParsedDeclSpec.h"
#include "ParserErrorManager.h"
#include "scc/Frontend/LangOpt.h"
#include "scc/PreProcessor/PreProcessor.h"
#include "scc/Sema/Sema.h"
#include "scc/Token/Token.h"

namespace scc {

class Parser {
    PreProcessor       &PP;
    Sema               &Action;
    ParserErrorManager &EM;
    const LangOpt      &Opts;

    bool IsEOF = false;
    bool HasErrorOccurred = false;

    Token CurTok;
    Token LookaheadTok;
    bool  HasLookaheadTok = false;

  public:
    Parser(PreProcessor &PP, Sema &Action, ParserErrorManager &EM, const LangOpt &Opts)
        : PP(PP),
          Action(Action),
          EM(EM),
          Opts(Opts) {}

    PreProcessor       &getPreprocessor() { return PP; }
    const PreProcessor &getPreprocessor() const { return PP; }

    Sema          &getSema() { return Action; }
    const Sema    &getSema() const { return Action; }
    const LangOpt &getLangOpt() const { return Opts; }

    bool isEOF() { return IsEOF; }
    bool hasErrorOccurred() { return HasErrorOccurred; }

    // Parse one top-level declaration (global/file scope, outside functions),
    // e.g. variable (int x;), function (int foo(){}), struct (struct S{}), typedef, etc.
    // Returns the resulting Decl, or nullptr on error.
    DeclList parseTopLevelDecl();

    ParsedDeclSpec   parseDeclSpec();
    ParsedDeclarator parseDeclarator();
    DeclList         parseDeclaration();

  private:
    bool next() {
        if (HasLookaheadTok) {
            CurTok = LookaheadTok;
            LookaheadTok.flush();
            HasLookaheadTok = false;
            if (CurTok.getTokenKind() == tok::eof)
                IsEOF = true;
            return CurTok.getTokenKind() == tok::eof;
        }

        bool ret = PP.next(CurTok);
        // Temporary token-state plumbing; this will go away when parser recovery is
        // redesigned and EOF/error handling is not folded into one bool.
        if (CurTok.getTokenKind() == tok::eof)
            IsEOF = true;
        return ret;
    }

    const Token &peek() {
        if (!HasLookaheadTok) {
            PP.next(LookaheadTok);
            HasLookaheadTok = true;
        }
        return LookaheadTok;
    }

    bool consumeIf(tok::TokenKind TK);
    bool expect(tok::TokenKind TK);
    void skipUntilDeclarationEnd();

    bool isType(tok::TokenKind TK);
};

} // namespace scc

#endif // SCC_PARSER_PARSER_H
