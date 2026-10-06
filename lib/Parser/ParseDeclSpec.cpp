#include "scc/Parser/Parser.h"
#include "scc/Sema/ParsedDeclSpec.h"

using namespace scc;

static bool unknown_identifier_looks_like_type_specifier(const Token &Next) {
    return Next.is(tok::identifier, tok::star, tok::l_paren);
}


ParsedDeclSpec Parser::parseDeclSpec() {
    ParsedDeclSpec DS;

    while (true) {
        switch (CurTok.getTokenKind()) {
        case tok::identifier: {
            if (DS.hasTypeSpecifier())
                return DS;

            const Type *T = Action.getTypeSpecifierType(CurTok);
            if (T->isUnknow() && !unknown_identifier_looks_like_type_specifier(peek())) {
                DS.setTypeSpecifier(nullptr, CurTok.getRange(), CurTok.getValue());
                return DS;
            }

            DS.setTypeSpecifier(T, CurTok.getRange(), CurTok.getValue());
            break;
        }
        case tok::star:
        case tok::comma:
        case tok::kw_struct:
        case tok::kw_union:
            return DS;

        case tok::t_char:
        case tok::t_int:
        case tok::t_float:
        case tok::t_double:
        case tok::t_void:
        case tok::t__Bool: {
            const Type *T = Action.getType(CurTok);
            if (!DS.tryAddTypeSpecifier(T, CurTok.getRange())) {
                tok::TokenKind Previous = CurTok.getTokenKind();
                if (DS.T && DS.T->isBuiltinType()) {
                    Previous = static_cast<tok::TokenKind>(
                        static_cast<const BuiltinType *>(DS.T)->getBuiltinKind());
                }

                EM.cannotCombine(CurTok.getTokenKind(), Previous, CurTok.getRange())
                    .msg(" declaration specifier");
                HasErrorOccurred = EM.emit();
            }
            break;
        }

        case tok::t__Imaginary:
        case tok::t__Complex: {
            // Temporary until full type-specifier parsing can combine these
            // correctly with float/double/long double.
            EM.todo("imaginary and complex identifier", CurTok.getRange(), err::warning)
                .msg(" ignored for now");
            break;
        };

        case tok::t_signed:
        case tok::t_unsigned: {
            SignSpecifier New = static_cast<SignSpecifier>(CurTok.getTokenKind());
            if (!DS.tryAddSignSpecifier(New, CurTok.getRange())) {
                EM.cannotCombine(New, DS.getSignSpecifier(), CurTok.getRange())
                    .msg(" declaration specifier");
                HasErrorOccurred = EM.emit();
            }
            break;
        }

        case tok::t_long:
        case tok::t_short: {
            LengthSpecifier New = static_cast<LengthSpecifier>(CurTok.getTokenKind());
            if (!DS.tryAddLengthSpecifier(New, CurTok.getRange())) {
                EM.cannotCombine(New, DS.getLengthSpecifier(), CurTok.getRange())
                    .msg(" declaration specifier");
                HasErrorOccurred = EM.emit();
            }
            break;
        }

        case tok::kw_auto:
        case tok::kw_typedef:
        case tok::kw_static:
        case tok::kw_extern:
        case tok::kw_register: {
            StorageClassSpecifier New = static_cast<StorageClassSpecifier>(CurTok.getTokenKind());
            if (!DS.tryAddStorageSpecifier(New, CurTok.getRange())) {
                EM.cannotCombine(CurTok.getTokenKind(), DS.getStorageSpecifierTokenKind(),
                                 CurTok.getRange())
                    .msg(" declaration specifier");
                HasErrorOccurred = EM.emit();
            }
            break;
        }

        case tok::kw_const:
            if (!DS.tryAddConst()) {
                EM.duplicateQualifier(CurTok.getTokenKind(), CurTok.getRange())
                    .msg(" declaration specifier");
                HasErrorOccurred = EM.emit();
            }
            break;

        case tok::kw_restrict:
            if (!DS.tryAddRestrict()) {
                EM.duplicateQualifier(CurTok.getTokenKind(), CurTok.getRange())
                    .msg(" declaration specifier");
                HasErrorOccurred = EM.emit();
            }
            break;

        case tok::kw_volatile:
            if (!DS.tryAddVolatile()) {
                EM.duplicateQualifier(CurTok.getTokenKind(), CurTok.getRange())
                    .msg(" declaration specifier");
                HasErrorOccurred = EM.emit();
            }
            break;

        default:
            // For now, only diagnose bad lexer tokens here. The caller/future
            // recovery code will decide whether to consume or synchronize.
            if (CurTok.is(tok::unknown)) {
                EM.unknownToken(CurTok).msg(" in declaration specifier");
                HasErrorOccurred = EM.emit();
            }
            return DS;
        }
        if (next())
            break;
    }

    IsEOF = true;
    return DS;
}
