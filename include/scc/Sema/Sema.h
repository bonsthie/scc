#ifndef SCC_SEMA_SEMA_H
#define SCC_SEMA_SEMA_H

#include <string_view>

#include "scc/AST/ASTContext.h"
#include "scc/AST/BuiltinType.h"
#include "scc/AST/CanQualType.h"
#include "scc/AST/Decl.h"
#include "scc/AST/Type.h"
#include "scc/Error/ErrorManager.h"
#include "scc/Frontend/LangOpt.h"
#include "scc/Parser/ParseDeclarator.h"
#include "scc/Parser/ParsedDeclSpec.h"
#include "scc/Sema/Scope.h"
#include "scc/Token/Token.h"

namespace scc {

class Sema {
    [[maybe_unused]] ASTContext   &Ctx;
    [[maybe_unused]] ErrorManager &EM;
    const LangOpt                 &Opts;

    ScopeMgr SM;

  public:
    Sema(ASTContext &Ctx, ErrorManager &EM, const LangOpt &Opts) : Ctx(Ctx), EM(EM), Opts(Opts) {}

    void pushScope() { SM.newScope(); }
    void popScope() { SM.popScope(); }

    ASTContext       &getASTContext() { return Ctx; }
    const ASTContext &getASTContext() const { return Ctx; }
    const LangOpt    &getLangOpt() const { return Opts; }

    bool  addDecl(std::string_view Name, Decl *D) { return SM.addDecl(Name, D); }
    Decl *lookup(std::string_view Name) { return SM.lookupDecl(Name); }

    Decl *actOnDeclarator(ParsedDeclSpec &DS, ParsedDeclarator &D);

    const Type        *getType(Token &T);
    const Type        *getTypeSpecifierType(Token &T);
    const CanQualType *getBuiltinType(BuiltinTypeKind BType);
};

} // namespace scc

#endif // SCC_SEMA_SEMA_H
