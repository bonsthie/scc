#include "scc/Parser/ParserErrorManager.h"
#include "scc/Parser/ParsedDeclSpec.h"

using namespace scc;

static std::string tokenForDiag(const Token &Tok) {
    if (!Tok.getValue().empty())
        return std::string(Tok.getValue());
    return token_spelling(Tok.getTokenKind());
}

static Error &atRange(Error &Diag, const SourceRange &Range) {
    if (Range.isValid())
        Diag.at(Range.posViewBegin());
    return Diag;
}

static std::string signSpecifierForDiag(SignSpecifier Sign) {
    switch (Sign) {
    case SignSpecifier::Signed:
        return "signed";
    case SignSpecifier::Unsigned:
        return "unsigned";
    case SignSpecifier::Unspecified:
        return "<unspecified-sign>";
    }
    return "<invalid-sign>";
}

static std::string lengthSpecifierForDiag(LengthSpecifier Length) {
    switch (Length) {
    case LengthSpecifier::Short:
        return "short";
    case LengthSpecifier::Long:
        return "long";
    case LengthSpecifier::LongLong:
        return "long long";
    case LengthSpecifier::Unspecified:
        return "<unspecified-length>";
    }
    return "<invalid-length>";
}

Error &ParserErrorManager::expectedXButGotY(tok::TokenKind Expected, const Token &Got) {
    return report(err::error)
        .at(Got.posViewBegin())
        .msg("expected '")
        .msg(token_spelling(Expected))
        .msg("' but got '")
        .msg(tokenForDiag(Got))
        .msg("'");
}

Error &ParserErrorManager::duplicateQualifier(tok::TokenKind Qualifier, const SourceRange &Range) {
    return atRange(report(err::error), Range)
        .msg("duplicate Qualifer '")
        .msg(token_spelling(Qualifier))
        .msg("'");
}

Error &ParserErrorManager::cannotCombine(tok::TokenKind X, tok::TokenKind Y,
                                         const SourceRange &Range) {
    return atRange(report(err::error), Range)
        .msg("cannot combine '")
        .msg(token_spelling(X))
        .msg("' with previous '")
        .msg(token_spelling(Y))
        .msg("'");
}

Error &ParserErrorManager::cannotCombine(SignSpecifier X, SignSpecifier Y,
                                         const SourceRange &Range) {
    return atRange(report(err::error), Range)
        .msg("cannot combine '")
        .msg(signSpecifierForDiag(X))
        .msg("' with previous '")
        .msg(signSpecifierForDiag(Y))
        .msg("'");
}

Error &ParserErrorManager::cannotCombine(LengthSpecifier X, LengthSpecifier Y,
                                         const SourceRange &Range) {
    return atRange(report(err::error), Range)
        .msg("cannot combine '")
        .msg(lengthSpecifierForDiag(X))
        .msg("' with previous '")
        .msg(lengthSpecifierForDiag(Y))
        .msg("'");
}
