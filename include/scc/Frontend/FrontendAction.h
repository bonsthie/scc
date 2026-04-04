#ifndef SCC_FRONTEND_FRONTENDACTION
#define SCC_FRONTEND_FRONTENDACTION

#include "scc/AST/ASTConsumer.h"
#include "scc/Frontend/CompilerInstance.h"
#include "scc/Parser/Parser.h"
#include <memory>

namespace scc {

bool ParseAST(Parser &P, ASTConsumer &Consumer, ASTContext &Context);

class FrontendAction {
  public:
    virtual ~FrontendAction() = default;
    virtual bool Execute(CompilerInstance &CI) = 0;
};

class DumpToken : public FrontendAction {
    bool Execute(CompilerInstance &CI) {
        auto &PP = CI.getPreprocessor();
        auto &EM = CI.getErrorManager();

        Token CurTok;
        do {
            CurTok.flush();
            PP.next(CurTok);
            std::cout << CurTok << std::endl;
            if (EM.emit())
                return true;
        } while (!CurTok.is(tok::eof));
        return false;
    }
};

class DumpRawToken : public FrontendAction {
    bool Execute(CompilerInstance &CI) {
        auto &PP = CI.getPreprocessor();
        auto &EM = CI.getErrorManager();

        Token CurTok;
        do {
            CurTok.flush();
            PP.nextRaw(CurTok);
            std::cout << CurTok << std::endl;
            if (EM.emit())
                return true;
        } while (!CurTok.is(tok::eof));
        return false;
    }
};

class ASTFrontendAction : public FrontendAction {
  public:
    bool Execute(CompilerInstance &CI) final {
        std::unique_ptr<ASTConsumer> Consumer = CreateConsumer(CI);
        if (!Consumer)
            return false;

        CI.InitSema(*Consumer);

        Parser P(CI.getPreprocessor(), CI.getSema());
        return ParseAST(P, *Consumer, CI.getASTContext());
    }

  protected:
    virtual std::unique_ptr<ASTConsumer> CreateConsumer(CompilerInstance &CI) = 0;
};

class DumpASTAction final : public ASTFrontendAction {
  protected:
    std::unique_ptr<ASTConsumer> CreateConsumer(CompilerInstance &) override {
        return std::make_unique<DumpASTConsumer>();
    }
};

} // namespace scc

#endif // SCC_FRONTEND_FRONTENDACTION
