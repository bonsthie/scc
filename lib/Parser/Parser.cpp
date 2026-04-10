#include "scc/Parser/Parser.h"
#include "scc/ADT/vector.h"
#include "scc/Parser/ParsedDeclSpec.h"

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

bool isType(tok::TokenKind TK) {
    switch (TK) {
#define TYPE_KEYWORD(X) case tok::t_##X:
#include "scc/Token/TokenKinds.def"
        return true;
    default:
        return false;
    }
}

DeclList Parser::parseTopLevelDecl() {
    next(); // init fist token
    auto DS = parseDeclSpec();
    DS.print();
    IsEOF = true;
    return {};
}

ParsedDeclSpec Parser::parseDeclSpec() {
    ParsedDeclSpec DS;

    while (true) {
        switch (CurTok.getTokenKind()) {
        case tok::identifier: {
            if (DS.hasTypeSpecifier())
                return DS; // Main exit

            const Type *T = Action.getTypeSpecifierType(CurTok);
            DS.setTypeSpecifier(T, CurTok.getRange());
            break;
        }

        case tok::t_char:
        case tok::t_int:
        case tok::t_float:
        case tok::t_double:
        case tok::t_void:
        case tok::t__Bool:
        case tok::t__Imaginary:
        case tok::t__Complex: {
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
            if (!DS.tryAddStorageSpecifier(New)) {
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
            return DS;
        }
        if (next())
            break;
    }

    IsEOF = true;
    return DS;
}

ParsedDeclarator Parser::parseDeclarator() { return ParsedDeclarator(); }

DeclList Parser::parseDeclaration() {
    SmallVector<Decl *, 4> Decls;

    ParsedDeclSpec DS = parseDeclSpec();
    if (hasErrorOccurred())
        return {};

    do {
        ParsedDeclarator D = parseDeclarator();

        Decl *Res = Action.actOnDeclarator(DS, D);
        if (Res)
            Decls.push_back(Res);
    } while (consumeIf(tok::comma));

    if (!expect(tok::semi))
        return {};

    return Action.getASTContext().toOwnedList(Decls);
}
