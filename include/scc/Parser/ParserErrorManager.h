#ifndef SCC_PARSER_PARSERERRORMANAGER_H
#define SCC_PARSER_PARSERERRORMANAGER_H

#include "scc/Frontend/FrontendErrorManager.h"
#include "scc/Token/Token.h"
#include <concepts>
#include <cstddef>

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
    template <typename... ExpectedKinds>
        requires(sizeof...(ExpectedKinds) > 0 &&
                 (std::same_as<ExpectedKinds, tok::TokenKind> && ...))
    Error &expectedX(const PosView Pos, ExpectedKinds... Kinds) {
        const tok::TokenKind Expected[] = {Kinds...};
        constexpr std::size_t Count = sizeof...(Kinds);

        Error &Diag = report(err::error).at(Pos).msg("expected ");
        for (std::size_t I = 0; I < Count; ++I) {
            if (I != 0)
                Diag.msg(I == Count - 1 ? " or " : ", ");
            if (Expected[I] == tok::identifier)
                Diag.msg("identifier");
            else
                Diag.msg("'").msg(token_spelling(Expected[I])).msg("'");
        }
        return Diag;
    }
    Error &toClose(const Token OpenTok);
    Error &unknownToken(const Token &Tok) { return EM.unknownToken(Tok); }
    Error &duplicateQualifier(tok::TokenKind Qualifier, const SourceRange &Range);
    Error &cannotCombine(tok::TokenKind X, tok::TokenKind Y, const SourceRange &Range);
    Error &cannotCombine(SignSpecifier X, SignSpecifier Y, const SourceRange &Range);
    Error &cannotCombine(LengthSpecifier X, LengthSpecifier Y, const SourceRange &Range);
};

} // namespace scc

#endif // SCC_PARSER_PARSERERRORMANAGER_H
