#ifndef SCC_AST_ASTCONSUMER
#define SCC_AST_ASTCONSUMER

#include "scc/AST/Decl.h"

namespace scc {

class ASTConsumer {
  public:
    virtual ~ASTConsumer();

    virtual bool HandleTopLevelDecl(Decl *D) = 0;
};

class DumpASTConsumer final : public ASTConsumer {

    bool HandleTopLevelDecl(Decl *) { return false; }
};

} // namespace scc

#endif // SCC_AST_ASTCONSUMER
