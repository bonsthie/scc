#ifndef SCC_PARSER_PARSERERRORMANAGER_H
#define SCC_PARSER_PARSERERRORMANAGER_H

#include "scc/Frontend/FrontendErrorManager.h"
#include "scc/Token/Token.h"

namespace scc {

enum class SignSpecifier : int;
enum class LengthSpecifier : int;

class ParserErrorManager {
    FrontendErrorManager &EM;

  public:
    explicit ParserErrorManager(FrontendErrorManager &EM) : EM(EM) {}

    FrontendErrorManager       &getErrorManager() { return EM; }
    const FrontendErrorManager &getErrorManager() const { return EM; }

    Error &report(err::DiagLevel Level) { return EM.report(Level); }
    Error &last() { return EM.last(); }
    bool   emit() { return EM.emit(); }

    Error &todo(std::string Thing, const SourceRange &Range, err::DiagLevel Level = err::error) {
        return EM.todo(Thing, Range, Level);
    }

    Error &expectedXButGotY(tok::TokenKind Expected, const Token &Got);
    Error &unknownToken(const Token &Tok) { return EM.unknownToken(Tok); }
    Error &duplicateQualifier(tok::TokenKind Qualifier, const SourceRange &Range);
    Error &cannotCombine(tok::TokenKind X, tok::TokenKind Y, const SourceRange &Range);
    Error &cannotCombine(SignSpecifier X, SignSpecifier Y, const SourceRange &Range);
    Error &cannotCombine(LengthSpecifier X, LengthSpecifier Y, const SourceRange &Range);
};

} // namespace scc

#endif // SCC_PARSER_PARSERERRORMANAGER_H
