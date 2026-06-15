#ifndef SCC_FRONTEND_FRONTENDACTION_H
#define SCC_FRONTEND_FRONTENDACTION_H

#include "scc/AST/ASTConsumer.h"
#include "scc/Frontend/CompilerInstance.h"
#include "scc/Parser/Parser.h"
#include <memory>

namespace scc {

bool parse_ast(Parser &P, ASTConsumer &Consumer);

class FrontendAction {
  public:
    virtual ~FrontendAction() = default;
    virtual bool execute(CompilerInstance &CI) = 0;
};

class DumpToken : public FrontendAction {
  public:
    bool execute(CompilerInstance &CI) {
        auto &PP = CI.getPreprocessor();
        auto &EM = CI.getErrorManager();

        Token CurTok;
        do {
            CurTok.flush();
            PP.next(CurTok);
            if (!EM.emit())
                std::cout << CurTok << std::endl;
        } while (!CurTok.is(tok::eof));
        return false;
    }
};

class DumpRawToken : public FrontendAction {
  public:
    bool execute(CompilerInstance &CI) {
        auto &PP = CI.getPreprocessor();
        auto &EM = CI.getErrorManager();

        Token CurTok;
        do {
            CurTok.flush();
            PP.nextRaw(CurTok);
            if (!EM.emit())
                std::cout << CurTok << std::endl;
        } while (!CurTok.is(tok::eof));
        return false;
    }
};

class ASTFrontendAction : public FrontendAction {
  public:
    bool execute(CompilerInstance &CI) final {
        std::unique_ptr<ASTConsumer> Consumer = createConsumer(CI);
        if (!Consumer)
            return false;

        CI.initSema();

        ParserErrorManager PEM(CI.getErrorManager());
        Parser             P(CI.getPreprocessor(), CI.getSema(), PEM, CI.getLangOpt());
        return parse_ast(P, *Consumer);
    }

  protected:
    virtual std::unique_ptr<ASTConsumer> createConsumer(CompilerInstance &CI) = 0;
};

class DumpASTAction final : public ASTFrontendAction {
  protected:
    std::unique_ptr<ASTConsumer> createConsumer(CompilerInstance &) override {
        return std::make_unique<DumpASTConsumer>();
    }
};

class SyntaxOnly final : public ASTFrontendAction {
  protected:
    std::unique_ptr<ASTConsumer> createConsumer(CompilerInstance &) override {
        return std::make_unique<SyntaxOnlyConsumer>();
    }
};

} // namespace scc

#endif // SCC_FRONTEND_FRONTENDACTION_H
