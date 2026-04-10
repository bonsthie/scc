#include "scc/Sema/Scope.h"
#include "scc/AST/Type.h"

using namespace scc;

bool ScopeMgr::addDecl(std::string_view Name, Decl *D) { return DeclScopes.back().add(Name, D); }
bool ScopeMgr::addType(std::string_view Name, Type *D) { return TypeScopes.back().add(Name, D); }

static Type UnknowType(TypeKind::Unknow);

Decl *ScopeMgr::lookupDecl(std::string_view Name) {
    for (auto It = DeclScopes.rbegin(); It != DeclScopes.rend(); ++It) {
        if (Decl *D = It->lookup(Name))
            return D;
    }
    return nullptr;
}

Type *ScopeMgr::lookupTypeOrNull(std::string_view Name) {
    for (auto It = TypeScopes.rbegin(); It != TypeScopes.rend(); ++It) {
        if (Type *D = It->lookup(Name))
            return D;
    }
    return nullptr;
}

Type *ScopeMgr::lookupType(std::string_view Name) {
    if (Type *D = lookupTypeOrNull(Name))
        return D;
    return &UnknowType;
}
