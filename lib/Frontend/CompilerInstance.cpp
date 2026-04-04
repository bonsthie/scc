#include "scc/Frontend/CompilerInstance.h"

#include "scc/Frontend/FrontendAction.h"

using namespace scc;

CompilerInstance::CompilerInstance(FileManager &FM, ErrorManager &EM, CompilerInstanceSettings,
                                   StringInterner &SI, std::unique_ptr<FrontendAction> Act)
    : SI(&SI),
      Act(std::move(Act)),
      EM(EM),
      FM(&FM),
      Context(std::make_unique<ASTContext>()) {}

CompilerInstance::~CompilerInstance() = default;

bool CompilerInstance::Execute() { return Act->Execute(*this); }
