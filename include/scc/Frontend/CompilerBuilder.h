#ifndef SCC_COMPILERBUILDER_H
#define SCC_COMPILERBUILDER_H

#include "CompilerInstance.h"
#include "scc/Error/ErrorManager.h"
#include "scc/FileManager/FileManager.h"
#include "scc/Option/Args.h"
namespace scc {

class CompilerBuilder {
    FileManager  &FM;
    ErrorManager &EM;
    ArgsList     &Args;

    CompilerBuilder(FileManager &FM, ErrorManager &EM, ArgsList &Args)
        : FM(FM),
          EM(EM),
          Args(Args) {}

    CompilerInstance *create();
};

} // namespace scc

#endif // SCC_COMPILERBUILDER_H
