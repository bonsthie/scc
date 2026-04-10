#include "scc/Sema/Sema.h"
#include "scc/AST/BuiltinType.h"
#include "scc/Token/Token.h"

#include <cassert>

using namespace scc;

Decl *Sema::actOnDeclarator(ParsedDeclSpec &DS, ParsedDeclarator &D) {
    (void)DS;
    (void)D;
    return nullptr;
}

const Type *Sema::getType(Token &T) {
    switch (T.getTokenKind()) {
    case tok::t_char:
    case tok::t_int:
    case tok::t_float:
    case tok::t_double:
    case tok::t_void:
    case tok::t__Bool:
    case tok::t__Imaginary:
    case tok::t__Complex:
        return getBuiltinType(static_cast<BuiltinTypeKind>(T.getTokenKind()))
            ->asQualType()
            .getType();
    case tok::identifier:
        return SM.lookupType(T.getValue());
    default:
        assert(false &&
               "Sema::getType should only be called for builtin type tokens or identifiers");
        return nullptr;
    }
}

const Type *Sema::getTypeSpecifierType(Token &T) {
    if (T.getTokenKind() == tok::identifier)
        return SM.lookupTypeOrNull(T.getValue());
    return getType(T);
}
