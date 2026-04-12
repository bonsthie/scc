#ifndef SCC_FRONTEND_FRONTENDERRORMANAGER_H
#define SCC_FRONTEND_FRONTENDERRORMANAGER_H

#include <string>
#include <string_view>

#include "scc/Error/ErrorManager.h"
#include "scc/Token/Token.h"

namespace scc {

class FrontendErrorManager : public ErrorManager {
  public:
    using ErrorManager::ErrorManager;

    static Error &atRange(Error &Diag, const SourceRange &Range) {
        if (Range.isValid())
            Diag.at(Range.posViewBegin());
        return Diag;
    }

    Error &todo(std::string Thing, const SourceRange &Range, err::DiagLevel Level = err::error) {
        return atRange(report(Level), Range).msg("option ").quote(Thing).msg(" Not implemented yet");
    }

    Error &unknownToken(const Token &Tok) {
        return report(err::error)
            .at(Tok.posViewBegin())
            .msg("unknown token '")
            .msg(tokenForDiag(Tok))
            .msg("'");
    }

    Error &duplicateQualifier(std::string_view Qualifier, const SourceRange &Range) {
        return atRange(report(err::error), Range)
            .msg("duplicate Qualifer '")
            .msg(std::string(Qualifier))
            .msg("'");
    }

    Error &cannotCombine(std::string_view X, std::string_view Y, const SourceRange &Range) {
        return atRange(report(err::error), Range)
            .msg("cannot combine '")
            .msg(std::string(X))
            .msg("' with previous '")
            .msg(std::string(Y))
            .msg("'");
    }

    Error &unknownTypeName(std::string_view Name, const SourceRange &Range) {
        Error &Diag = atRange(report(err::error), Range).msg("unknown type name");
        if (!Name.empty())
            Diag.msg(" '").msg(std::string(Name)).msg("'");
        return Diag;
    }

    Error &missingTypeSpecifier(const SourceRange &Range) {
        return atRange(report(err::error), Range)
            .msg("type specifier missing, defaults to 'int'; ISO C99 and later do not "
                 "support implicit int [-Wimplicit-int]");
    }

    Error &invalidTypeModifier(std::string_view Modifier, std::string_view TypeName,
                               const SourceRange &Range) {
        return atRange(report(err::error), Range)
            .msg("'")
            .msg(std::string(Modifier))
            .msg("' is invalid with '")
            .msg(std::string(TypeName))
            .msg("'");
    }

  private:
    static std::string tokenForDiag(const Token &Tok) {
        if (!Tok.getValue().empty())
            return std::string(Tok.getValue());
        return token_spelling(Tok.getTokenKind());
    }
};

} // namespace scc

#endif // SCC_FRONTEND_FRONTENDERRORMANAGER_H
