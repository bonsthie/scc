#ifndef SCC_AST_ASTCONSUMER
#define SCC_AST_ASTCONSUMER

#include "scc/AST/Decl.h"
#include <iostream>

namespace scc {

class ASTConsumer {
  public:
    virtual ~ASTConsumer();

    virtual bool handleTopLevelDecl(DeclList Decls) = 0;
};

class DumpASTConsumer final : public ASTConsumer {
  public:
    bool handleTopLevelDecl(DeclList Decls) override {
        if (Decls.empty())
            return false;

        for (Decl *D : Decls) {
            if (!D)
                continue;
            D->print();
        }

        return false;
    }
};

} // namespace scc

#endif // SCC_AST_ASTCONSUMER
