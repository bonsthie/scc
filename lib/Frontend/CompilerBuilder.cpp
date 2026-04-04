#include "scc/Frontend/CompilerBuilder.h"

#include "scc/Error/Error.h"

using namespace scc;

CompilerInstance *CompilerBuilder::create() { return create("test.c"); }

CompilerInstance *CompilerBuilder::create(File *MainFile) {

    auto Act = selectForntendAction();

    auto *CI = new CompilerInstance(FM, EM, CompilerInstanceSettings{}, SI,
                                    std::unique_ptr<FrontendAction>(Act));
    if (MainFile)
        CI->createPreprocessor(*MainFile);
    return CI;
}

CompilerInstance *CompilerBuilder::create(const std::string &MainFileName) {
    File *MainFile = FM.getFile(MainFileName);
    if (!MainFile) {
        EM.report(err::error).msg("unable to open input file '").msg(MainFileName).msg("'");
        return nullptr;
    }

    return create(MainFile);
}

FrontendAction *CompilerBuilder::selectForntendAction() {
    (void)Args;
    return new DumpToken();
}
