#include "scc/Frontend/CompilerInstance.h"

#include "scc/Frontend/FrontendAction.h"
#include "scc/AST/ASTContext.h"

using namespace scc;

CompilerInstance::CompilerInstance(FileManager &FM, ErrorManager &EM, CompilerInstanceSettings,
                                   StringInterner &SI, std::unique_ptr<FrontendAction> Act)
    : SI(&SI),
      Act(std::move(Act)),
      EM(EM),
      FM(&FM),
      Ctx(std::make_unique<ASTContext>()) {}

CompilerInstance::~CompilerInstance() = default;

bool CompilerInstance::Execute() { return Act->Execute(*this); }
