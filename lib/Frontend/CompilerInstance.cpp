#include "scc/Frontend/CompilerInstance.h"

#include "scc/AST/ASTContext.h"
#include "scc/Frontend/FrontendAction.h"

using namespace scc;

CompilerInstance::CompilerInstance(FileManager &FM, FrontendErrorManager &EM,
                                   CompilerInstanceSettings Settings, StringInterner &SI,
                                   std::unique_ptr<FrontendAction> Act)
    : SI(&SI),
      Act(std::move(Act)),
      Settings(Settings),
      EM(EM),
      FM(&FM),
      Ctx(std::make_unique<ASTContext>()) {}

CompilerInstance::~CompilerInstance() = default;

bool CompilerInstance::execute() { return Act->execute(*this); }
