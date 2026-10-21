
#include "scc/Colors/Colors.h"
#include "scc/Parser/Parser.h"
#include "scc/Sema/ParsedDeclarator.h"
#include "scc/Token/Token.h"
#include <optional>
#include <ostream>

using namespace scc;

/*
declarator
    := pointer? direct_declarator

pointer
    := '*' qualifiers? pointer?

direct_declarator
    := IDENT
     | '(' declarator ')'

direct_declarator
    := direct_declarator '[' ... ']'
     | direct_declarator '(' ... ')'
*/

// int **(*(*(f)[]))[]
//
// outer declarator:
// ** (...) []
//
// recurse into (...):
//      * (...)
//
// recurse again:
//          * (f) []
//
// base:
//              f

// fist token of the Declarator should be in CurTok
ParsedDeclarator Parser::parseDeclarator() {
    ParsedDeclarator D;

    parseDeclaratorInternal(D);
    D.dump();
    return D;
}

bool Parser::parseDeclaratorInternal(ParsedDeclarator &D) {
    auto Pointer = parseDeclaratorPrefixPointer();

    switch (CurTok.getTokenKind()) {
    case tok::star: {
        if (parseDeclaratorInternal(D))
            return true;
        break;
    }

    case tok::l_paren: {
        auto LParen = CurTok;
        next();
        if (parseDeclaratorInternal(D))
            return true;
        if (!CurTok.is(tok::r_paren)) {
            EM.expectedX(CurTok.posViewBegin(), tok::r_paren);
            EM.toClose(LParen);
            return true;
        }
        next();
        break;
    }

    case tok::identifier: {
        D.setName(CurTok.getValue(), CurTok.getRange());
        next();
        break;
    }

    case tok::l_square:
        return parseMisplacedBracketDeclarator(D);

    default:
        EM.expectedX(CurTok.posViewBegin(), tok::identifier, tok::l_paren);
        return true;
    }

    while (auto Suffix = parseDeclaratorSufix())
        D.pushChunk(*Suffix);

    if (Pointer)
        D.pushChunk(*Pointer);

    return false;
}

std::optional<DeclaratorChunks> Parser::parseDeclaratorPrefixPointer() {
    if (!CurTok.is(tok::star))
        return std::nullopt;
    SourceRange Range = CurTok.getRange();
    next();
    Qualifiers Quals;
    parseQualifiers(Quals);
    return DeclaratorChunks(DeclPointerChunk{.Qual = Quals}, Range);
}

std::optional<DeclaratorChunks> Parser::parseDeclaratorSufix() {

    switch (CurTok.getTokenKind()) {
    default: {
        if (CurTok.is(tok::comma, tok::r_paren, tok::semi)) {
            return std::nullopt;
        }

        if (CurTok.is(tok::equal)) {
            EM.todo("assignation initialisation", CurTok.getRange());
        } else {
            EM.unknownToken(CurTok);
        }
        return std::nullopt;
    }

    case tok::l_paren:
        return parseDeclaratorSufixFunction();
    case tok::l_square:
        return parseDeclaratorSufixArray();
    }
}

std::optional<DeclaratorChunks> Parser::parseDeclaratorSufixArray() {
    next();
    if (CurTok.is(tok::r_square)) {
        DeclaratorChunks Decl(DeclArrayChunk{}, CurTok.getRange());
        next();
        return Decl;
    }

    EM.todo("parse decalrator suffix array [...]", CurTok.getRange());
    return std::nullopt;
}

std::optional<DeclaratorChunks> Parser::parseDeclaratorSufixFunction() {
    next();

    if (CurTok.is(tok::r_paren)) {
        DeclaratorChunks Decl(DeclFuncChunk{}, CurTok.getRange());
        next();
        return Decl;
    }

    EM.todo("parse decalrator function parameter (...)", CurTok.getRange());
    return std::nullopt;
}

bool Parser::parseMisplacedBracketDeclarator(ParsedDeclarator &) {
    EM.todo("parseMisplacedBracketDeclarator", CurTok.getRange(), err::error);
    return true;
}

void ParsedDeclarator::dump(std::ostream &O) const {
    O << Color::bold() << "ParsedDeclarator" << Color::reset() << " \"" << Color::bold() << Name
      << Color::reset() << '\"' << Color::dim() << " @ " << NameSourceRange.posViewBegin() << '-'
      << NameSourceRange.End.Line << ':' << NameSourceRange.End.Column << Color::reset() << '\n';
    for (const auto &Chunk : Chunks) {
        O << "  " << Color::bold();
        if (const auto *Pointer = std::get_if<DeclPointerChunk>(&Chunk.Type)) {
            O << "pointer" << Color::reset();
            if (Pointer->Qual.IsConst)
                O << " const";
            if (Pointer->Qual.IsRestrict)
                O << " restrict";
            if (Pointer->Qual.IsVolatile)
                O << " volatile";
        } else {
            const auto &Suffix = std::get<DeclSuffixType>(Chunk.Type);
            if (const auto *Array = std::get_if<DeclArrayChunk>(&Suffix)) {
                O << "array" << Color::reset() << " size=" << Array->Size.Num;
            } else {
                O << "function" << Color::reset()
                  << " args=" << std::get<DeclFuncChunk>(Suffix).FuncArgsDecl.Num;
            }
        }
        O << Color::dim() << " @ " << Chunk.Range.posViewBegin() << '-' << Chunk.Range.End.Line
          << ':' << Chunk.Range.End.Column << Color::reset() << '\n';
    }
}
