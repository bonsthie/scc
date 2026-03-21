#ifndef SCC_SEMA_SCOPE_H
#define SCC_SEMA_SCOPE_H

#include <string_view>
#include <unordered_map>
#include <vector>

#include "scc/AST/Decl.h"

namespace scc {

class Scope {
    std::unordered_map<std::string_view, Decl *> SymbolTable;

  public:
    Scope() = default;

    bool addDecl(std::string_view Name, Decl *D);

    Decl *lookup(std::string_view Name);
};

class ScopeMgr {
    std::vector<Scope> Scopes;

  public:
    void popScope() { Scopes.pop_back(); }
    void newScope() { Scopes.emplace_back(); }

    bool addDecl(std::string_view Name, Decl *D);

    Decl *lookup(std::string_view Name);
};

} // namespace scc

#endif // SCC_SEMA_SCOPE_H
