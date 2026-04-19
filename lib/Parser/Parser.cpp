#include "scc/Parser/Parser.h"
#include "scc/ADT/vector.h"
#include "scc/Error/Error.h"
#include "scc/Sema/ParsedDeclarator.h"
#include "scc/Sema/ParsedDeclSpec.h"

using namespace scc;

bool Parser::consumeIf(tok::TokenKind TK) {
    if (!CurTok.is(TK))
        return false;
    next();
    return true;
}

bool Parser::expect(tok::TokenKind TK) {
    if (!CurTok.is(TK)) {
        HasErrorOccurred = true;
        EM.expectedXButGotY(TK, CurTok);
        return false;
    }
    next();
    return true;
}

void Parser::skipUntilDeclarationEnd() {
    while (!CurTok.is(tok::semi, tok::eof)) {
        if (next())
            return;
    }
    if (CurTok.is(tok::semi))
        next();
}

bool isType(tok::TokenKind TK) {
    switch (TK) {
#define TYPE_KEYWORD(X) case tok::t_##X:
#include "scc/Token/TokenKinds.def"
        return true;
    default:
        return false;
    }
}

static bool unknownIdentifierLooksLikeTypeSpecifier(const Token &Next) {
    return Next.is(tok::identifier, tok::star, tok::l_paren);
}

DeclList Parser::parseTopLevelDecl() {
    if (CurTok.is(tok::not_init))
        next();
    if (CurTok.is(tok::eof))
        return {};
    return parseDeclaration();
}

ParsedDeclSpec Parser::parseDeclSpec() {
    ParsedDeclSpec DS;

    while (true) {
        switch (CurTok.getTokenKind()) {
        case tok::identifier: {
            if (DS.hasTypeSpecifier())
                return DS;

            const Type *T = Action.getTypeSpecifierType(CurTok);
            if (T->isUnknow() && !unknownIdentifierLooksLikeTypeSpecifier(peek())) {
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

// fist token of the Declarator should be in CurTok
ParsedDeclarator Parser::parseDeclarator() {
    ParsedDeclarator D;

    if (CurTok.is(tok::identifier)) {
		D.setName(CurTok.getValue(), CurTok.getRange());
    } else {
        EM.todo("parse declarator", CurTok.getRange());
    }
    return D;
}

DeclList Parser::parseDeclaration() {
    SmallVector<Decl *, 4> Decls;

    ParsedDeclSpec DS = parseDeclSpec();
    if (isEOF())
        return {};

    Action.actOnDeclSpec(DS);
    HasErrorOccurred |= EM.emit();

    if (hasErrorOccurred()) {
        skipUntilDeclarationEnd();
        return {};
    }

    do {
        ParsedDeclarator D = parseDeclarator();
        HasErrorOccurred |= EM.emit();
        if (hasErrorOccurred()) {
            skipUntilDeclarationEnd();
            return Action.getASTContext().toOwnedList(Decls);
        }

        Decl *Res = Action.actOnDeclarator(DS, D);
        if (Res)
            Decls.push_back(Res);
    } while (consumeIf(tok::comma) && !next());

    if (!expect(tok::semi))
        return {};

    return Action.getASTContext().toOwnedList(Decls);
}
