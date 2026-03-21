#ifndef SCC_SEMA_SEMA_H
#define SCC_SEMA_SEMA_H

#include <string_view>

#include "scc/Sema/Scope.h"

namespace scc {

class Sema {
    ScopeMgr SM;

  public:
    void pushScope() { SM.newScope(); }
    void popScope() { SM.popScope(); }

    bool addDecl(std::string_view Name, Decl *D) { return SM.addDecl(Name, D); }
    Decl *lookup(std::string_view Name) { return SM.lookup(Name); }
};

} // namespace scc

#endif // SCC_SEMA_SEMA_H
