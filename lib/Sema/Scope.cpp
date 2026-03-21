#include "scc/Sema/Scope.h"

using namespace scc;

bool Scope::addDecl(std::string_view Name, Decl *D) {
    if (SymbolTable.find(Name) != SymbolTable.end())
        return false;

    SymbolTable[Name] = D;
    return true;
}

Decl *Scope::lookup(std::string_view Name) {
    auto It = SymbolTable.find(Name);
    if (It != SymbolTable.end())
        return It->second;
    return nullptr;
}

bool ScopeMgr::addDecl(std::string_view Name, Decl *D) { return Scopes.back().addDecl(Name, D); }

Decl *ScopeMgr::lookup(std::string_view Name) {
    for (auto &Scope : Scopes) {
        if (Decl *D = Scope.lookup(Name))
            return D;
    }
    return nullptr;
}
