#include "scc/Parser/Parser.h"
#include "scc/ADT/vector.h"
#include "scc/Parser/ParsedDeclSpec.h"

using namespace scc;

std::string Parser::currentTokenForDiag() const {
    if (!CurTok.getValue().empty())
        return std::string(CurTok.getValue());
    return token_spelling(CurTok.getTokenKind());
}

bool Parser::consumeIf(tok::TokenKind TK) {
    if (!CurTok.is(TK))
        return false;
    next();
    return true;
}

bool Parser::expect(tok::TokenKind TK) {
    if (!CurTok.is(TK)) {
        HasErrorOccurred = true;
        EM.report(err::error)
            .at(CurTok.posViewBegin())
            .msg("expected '")
            .msg(token_spelling(TK))
            .msg("' but got '")
            .msg(currentTokenForDiag())
            .msg("'");
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

DeclList Parser::parseTopLevelDecl() { return {}; }

ParsedDeclSpec Parser::parseDeclSpec() {
    ParsedDeclSpec DS;

    do {
        switch (CurTok.getTokenKind())
        default:
            return DS;

    } while (next());

    IsEOF = true;
    return DS;
}

ParsedDeclarator Parser::parseDeclarator() { return ParsedDeclarator(); }

DeclList Parser::parseDeclaration() {
    SmallVector<Decl *, 4> Decls;

    ParsedDeclSpec DS = parseDeclSpec();

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
