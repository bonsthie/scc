#include "scc/Parser/Parser.h"
#include "scc/ADT/vector.h"
#include "scc/Sema/ParsedDeclSpec.h"
#include "scc/Sema/ParsedDeclarator.h"

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

DeclList Parser::parseTopLevelDecl() {
    if (CurTok.is(tok::not_init))
        next();
    if (CurTok.is(tok::eof))
        return {};
    return parseDeclaration();
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
            Decls.pushBack(Res);
    } while (consumeIf(tok::comma) && !next());

    if (!expect(tok::semi))
        return {};

    return Action.getASTContext().toOwnedList(Decls);
}
