#ifndef SCC_PARSER_PARSERERRORMANAGER_H
#define SCC_PARSER_PARSERERRORMANAGER_H

#include "scc/Error/ErrorManager.h"
#include "scc/Token/Token.h"

namespace scc {

enum class SignSpecifier : int;
enum class LengthSpecifier : int;

class ParserErrorManager {
    ErrorManager &EM;

  public:
    explicit ParserErrorManager(ErrorManager &EM) : EM(EM) {}

    ErrorManager       &getErrorManager() { return EM; }
    const ErrorManager &getErrorManager() const { return EM; }

    Error &report(err::DiagLevel Level) { return EM.report(Level); }
    Error &last() { return EM.last(); }
    bool   emit() { return EM.emit(); }

    Error &expectedXButGotY(tok::TokenKind Expected, const Token &Got);
    Error &duplicateQualifier(tok::TokenKind Qualifier, const SourceRange &Range);
    Error &cannotCombine(tok::TokenKind X, tok::TokenKind Y, const SourceRange &Range);
    Error &cannotCombine(SignSpecifier X, SignSpecifier Y, const SourceRange &Range);
    Error &cannotCombine(LengthSpecifier X, LengthSpecifier Y, const SourceRange &Range);
};

} // namespace scc

#endif // SCC_PARSER_PARSERERRORMANAGER_H
