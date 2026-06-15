#ifndef SCC_SEMA_SCOPE_H
#define SCC_SEMA_SCOPE_H

#include <string_view>
#include <unordered_map>

#include "scc/ADT/vector.h"

#include "scc/AST/Decl.h"

namespace scc {

template <typename T>
class Scope {
    std::unordered_map<std::string_view, T *> SymbolTable;

  public:
    Scope() = default;

    bool add(std::string_view Name, T *D) {
        if (SymbolTable.find(Name) != SymbolTable.end())
            return false;

        SymbolTable[Name] = D;
        return true;
    };

    T *lookup(std::string_view Name) {
        auto It = SymbolTable.find(Name);
        if (It != SymbolTable.end())
            return It->second;
        return nullptr;
    }
};

// maybe scope become SopeDecl and i create a ScopeType
// Scope become a std::pair<ScopeDecl, ScopeType> Scope or just a struct
// also `we need to think about the RecordScope mode

class ScopeMgr {
    scc::Vector<Scope<Decl>> DeclScopes;
    scc::Vector<Scope<Type>> TypeScopes;

  public:
    void popScope() {
        DeclScopes.popBack();
        TypeScopes.popBack();
    }
    void newScope() {
        DeclScopes.emplaceBack();
        TypeScopes.emplaceBack();
    }

    bool addDecl(std::string_view Name, Decl *D);
    bool addType(std::string_view Name, Type *D);

    Decl *lookupDecl(std::string_view Name);
    Type *lookupTypeOrNull(std::string_view Name);

    // info: return a unknow type if not found
    Type *lookupType(std::string_view Name);
};

} // namespace scc

#endif // SCC_SEMA_SCOPE_H
