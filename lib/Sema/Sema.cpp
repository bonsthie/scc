#include "scc/Sema/Sema.h"
#include "scc/AST/BuiltinType.h"
#include "scc/Token/Token.h"

#include <cassert>

using namespace scc;

Type *Sema::createTypeFromDeclarator(Type *T, ParsedDeclarator &D) {
    (void)T;
    (void)D;
    return nullptr;
}

Decl *Sema::createDeclFromDeclarator(ParsedDeclSpec &DS, ParsedDeclarator &D) {
    if (D.getName().empty()) {
        EM.report(err::error).msg("missing declarator name");
        return nullptr;
    }

    switch (DS.getStorageSpecifier()) {
    case StorageClassSpecifier::Typedef:

    case StorageClassSpecifier::Unspecified:

    case StorageClassSpecifier::Auto:
    case StorageClassSpecifier::Static:
    case StorageClassSpecifier::Extern:
    case StorageClassSpecifier::Register:
        EM.todo("storagespecifier not handle yet", DS.getStorageSpecifierRange());
        return nullptr;
    }
}

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
        return getType(static_cast<BuiltinTypeKind>(T.getTokenKind()));
    case tok::identifier:
        return SM.lookupType(T.getValue());
    default:
        assert(false &&
               "Sema::getType should only be called for builtin type tokens or identifiers");
        return nullptr;
    }
}

const Type *Sema::getType(BuiltinTypeKind Ty) { return getBuiltinType(Ty)->asQualType().getType(); }

// Unknown identifiers become the shared unknown/error type here;
// sema owns the final "unknown type name" diagnostic.
const Type *Sema::getTypeSpecifierType(Token &T) {
    if (T.getTokenKind() == tok::identifier)
        return SM.lookupType(T.getValue());
    return getType(T);
}
