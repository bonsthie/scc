#include "scc/Frontend/DeclSpecSpelling.h"
#include "scc/Parser/ParserErrorManager.h"
#include "scc/Sema/ParsedDeclSpec.h"

using namespace scc;

static std::string token_for_diag(const Token &Tok) {
    if (!Tok.getValue().empty())
        return std::string(Tok.getValue());
    return token_spelling(Tok.getTokenKind());
}

Error &ParserErrorManager::expectedXButGotY(tok::TokenKind Expected, const Token &Got) {
    return report(err::error)
        .at(Got.posViewBegin())
        .msg("expected '")
        .msg(token_spelling(Expected))
        .msg("' but got '")
        .msg(token_for_diag(Got))
        .msg("'");
}

Error &ParserErrorManager::duplicateQualifier(tok::TokenKind Qualifier, const SourceRange &Range) {
    return EM.duplicateQualifier(token_spelling(Qualifier), Range);
}

Error &ParserErrorManager::cannotCombine(tok::TokenKind X, tok::TokenKind Y,
                                         const SourceRange &Range) {
    return EM.cannotCombine(token_spelling(X), token_spelling(Y), Range);
}

Error &ParserErrorManager::cannotCombine(SignSpecifier X, SignSpecifier Y,
                                         const SourceRange &Range) {
    return EM.cannotCombine(sign_specifier_to_string(X), sign_specifier_to_string(Y), Range);
}

Error &ParserErrorManager::cannotCombine(LengthSpecifier X, LengthSpecifier Y,
                                         const SourceRange &Range) {
    return EM.cannotCombine(length_specifier_to_string(X), length_specifier_to_string(Y), Range);
}
