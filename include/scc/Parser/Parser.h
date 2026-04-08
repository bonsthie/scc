#ifndef SCC_PARSER_PARSER_H
#define SCC_PARSER_PARSER_H

#include "ParseDeclarator.h"
#include "ParsedDeclSpec.h"
#include "scc/Error/ErrorManager.h"
#include "scc/PreProcessor/PreProcessor.h"
#include "scc/Sema/Sema.h"
#include "scc/Token/Token.h"

namespace scc {

class Parser {
    PreProcessor                  &PP;
    Sema                          &Action;
    [[maybe_unused]] ErrorManager &EM;

    bool IsEOF = false;
    bool HasErrorOccurred = false;

    Token CurTok;

  public:
    Parser(PreProcessor &PP, Sema &Action, ErrorManager &EM) : PP(PP), Action(Action), EM(EM) {}

    PreProcessor       &getPreprocessor() { return PP; }
    const PreProcessor &getPreprocessor() const { return PP; }

    Sema       &getSema() { return Action; }
    const Sema &getSema() const { return Action; }

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
    bool next() { return PP.next(CurTok); }
    bool consumeIf(tok::TokenKind TK);
    bool expect(tok::TokenKind TK);

    bool isType(tok::TokenKind TK);

    std::string currentTokenForDiag() const;
};

} // namespace scc

#endif // SCC_PARSER_PARSER_H
