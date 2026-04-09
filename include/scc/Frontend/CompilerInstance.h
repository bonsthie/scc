#ifndef SCC_COMPILERINSTANCE_H
#define SCC_COMPILERINSTANCE_H

#include "scc/AST/ASTContext.h"
#include "scc/Frontend/CompilerInstanceSettings.h"
#include "scc/PreProcessor/PreProcessor.h"
#include "scc/Sema/Sema.h"
#include "scc/String/StringInterner.h"
#include <memory>
#include <utility>

namespace scc {

class ASTConsumer;
class FrontendAction;

class CompilerInstance {
    std::unique_ptr<Sema>           Action;
    std::unique_ptr<PreProcessor>   PP;
    StringInterner                 *SI;
    std::unique_ptr<FrontendAction> Act;
    CompilerInstanceSettings        Settings;
    ErrorManager                   &EM;
    FileManager                    *FM = nullptr;
    std::unique_ptr<ASTContext>     Ctx;

  public:
    CompilerInstance(FileManager &FM, ErrorManager &EM, CompilerInstanceSettings,
                     StringInterner &SI, std::unique_ptr<FrontendAction> Act);
    ~CompilerInstance();

    bool Execute();

    void InitSema() {
        if (!Ctx)
            Ctx = std::make_unique<ASTContext>();
        Action = std::make_unique<Sema>(*Ctx, EM, Settings.LangOptions);
    }

    void createPreprocessor(File &MainFile) {
        PP = std::make_unique<PreProcessor>(MainFile, EM, *FM, *SI, Settings.LangOptions);
    }

    PreProcessor       &getPreprocessor() { return *PP; }
    const PreProcessor &getPreprocessor() const { return *PP; }

    void setPreprocessor(std::unique_ptr<PreProcessor> NewPP) { PP = std::move(NewPP); }

    Sema       &getSema() { return *Action; }
    const Sema &getSema() const { return *Action; }

    ASTContext       &getASTContext() { return *Ctx; }
    const ASTContext &getASTContext() const { return *Ctx; }

    void setASTContext(std::unique_ptr<ASTContext> NewContext) { Ctx = std::move(NewContext); }

    FileManager       &getFileManager() { return *FM; }
    const FileManager &getFileManager() const { return *FM; }

    ErrorManager       &getErrorManager() { return EM; }
    const ErrorManager &getErrorManager() const { return EM; }

    LangOpt       &getLangOpt() { return Settings.LangOptions; }
    const LangOpt &getLangOpt() const { return Settings.LangOptions; }

    void setFileManager(FileManager &NewFM) { FM = &NewFM; }
};

} // namespace scc

#endif // SCC_COMPILERINSTANCE_H
