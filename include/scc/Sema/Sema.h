#ifndef SCC_SEMA_SEMA_H
#define SCC_SEMA_SEMA_H

#include <string_view>

#include "scc/AST/ASTContext.h"
#include "scc/AST/Decl.h"
#include "scc/AST/Type.h"
#include "scc/Error/ErrorManager.h"
#include "scc/Parser/ParseDeclarator.h"
#include "scc/Parser/ParsedDeclSpec.h"
#include "scc/Sema/Scope.h"
#include "scc/Token/Token.h"

namespace scc {

class Sema {
    [[maybe_unused]] ASTContext   &Ctx;
    [[maybe_unused]] ErrorManager &EM;

    ScopeMgr SM;

  public:
    Sema(ASTContext &Ctx, ErrorManager &EM) : Ctx(Ctx), EM(EM) {}

    void pushScope() { SM.newScope(); }
    void popScope() { SM.popScope(); }

    ASTContext       &getASTContext() { return Ctx; }
    const ASTContext &getASTContext() const { return Ctx; }

    bool  addDecl(std::string_view Name, Decl *D) { return SM.addDecl(Name, D); }
    Decl *lookup(std::string_view Name) { return SM.lookup(Name); }

    Decl *actOnDeclarator(ParsedDeclSpec &DS, ParsedDeclarator &D);

    Type *getType(Token &T);
};

} // namespace scc

#endif // SCC_SEMA_SEMA_H
