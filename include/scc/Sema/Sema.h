#ifndef SCC_SEMA_SEMA_H
#define SCC_SEMA_SEMA_H

#include <string_view>

#include "scc/AST/ASTContext.h"
#include "scc/Error/ErrorManager.h"
#include "scc/Sema/Scope.h"

namespace scc {

class Sema {
    [[maybe_unused]] ASTContext   &Ctx;
    [[maybe_unused]] ErrorManager &EM;

    ScopeMgr SM;

  public:
    Sema(ASTContext &Ctx, ErrorManager &EM) : Ctx(Ctx), EM(EM) {}

    void pushScope() { SM.newScope(); }
    void popScope() { SM.popScope(); }

    bool  addDecl(std::string_view Name, Decl *D) { return SM.addDecl(Name, D); }
    Decl *lookup(std::string_view Name) { return SM.lookup(Name); }
};

} // namespace scc

#endif // SCC_SEMA_SEMA_H
