#ifndef SCC_AST_ASTCONSUMER
#define SCC_AST_ASTCONSUMER

#include "scc/AST/Decl.h"
#include <iostream>

namespace scc {

class ASTConsumer {
  public:
    virtual ~ASTConsumer();

    virtual bool HandleTopLevelDecl(Decl *D) = 0;
};

class DumpASTConsumer final : public ASTConsumer {

    bool HandleTopLevelDecl(Decl *D) {
        if (!D)
            return false;

        D->print();
        return false;
    }
};

} // namespace scc

#endif // SCC_AST_ASTCONSUMER
