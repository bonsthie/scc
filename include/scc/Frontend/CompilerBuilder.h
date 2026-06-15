#ifndef SCC_FRONTEND_COMPILERBUILDER_H
#define SCC_FRONTEND_COMPILERBUILDER_H

#include <string>

#include "scc/Allocator/BumpAllocator.h"
#include "scc/FileManager/File.h"
#include "scc/FileManager/FileManager.h"
#include "scc/Frontend/CompilerInstance.h"
#include "scc/Frontend/FrontendAction.h"
#include "scc/Frontend/FrontendErrorManager.h"
#include "scc/Option/Args.h"

namespace scc {

class CompilerBuilder {
    BumpAllocator  Arena;
    StringInterner SI;
    FileManager   &FM;
    FrontendErrorManager &EM;
    ArgsList             &Args;

  public:
    CompilerBuilder(FileManager &FM, FrontendErrorManager &EM, ArgsList &Args)
        : Arena(),
          SI(Arena),
          FM(FM),
          EM(EM),
          Args(Args) {}


    CompilerInstance *create();
    CompilerInstance *create(File *MainFile);
    CompilerInstance *create(const std::string &MainFileName);

  private:
    FrontendAction *selectForntendAction();
};

} // namespace scc

#endif // SCC_FRONTEND_COMPILERBUILDER_H
