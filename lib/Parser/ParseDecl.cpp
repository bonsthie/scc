
#include "scc/Parser/Parser.h"
#include "scc/Sema/ParsedDeclarator.h"

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

    if (CurTok.is(tok::identifier)) {
		D.setName(CurTok.getValue(), CurTok.getRange());
    } else {
        EM.todo("parse declarator", CurTok.getRange());
    }
    return D;
}

