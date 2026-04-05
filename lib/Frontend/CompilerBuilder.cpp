#include "scc/Frontend/CompilerBuilder.h"

#include "scc/Error/Error.h"
#include "scc/Frontend/FrontendActionOptions.h"

using namespace scc;

CompilerInstance *CompilerBuilder::create() { return create("test.c"); }

CompilerInstance *CompilerBuilder::create(File *MainFile) {

    auto Act = selectForntendAction();
    if (Act == nullptr)
        return nullptr;

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
    const ArgOccurrence *Selected = nullptr;

    for (const auto &Occurrence : Args.getOccurrences()) {
        switch (Occurrence.getType()) {
        case Opt_dump_tokens:
        case Opt_dump_raw_tokens:
        case Opt_dump_ast:
            if (Selected == nullptr) {
                Selected = &Occurrence;
                break;
            }

            EM.report(err::error)
                .msg(Occurrence.getSpelling())
                .msg(" action ignored ")
                .msg(Selected->getSpelling())
                .msg(" action specified previously");
            break;
        default:
            break;
        }
    }

    if (EM.emit())
        return nullptr;

    if (Selected == nullptr)
        return nullptr;

    switch (Selected->getType()) {
    case Opt_dump_raw_tokens:
        return new DumpRawToken();
    case Opt_dump_ast:
        return new DumpASTAction();
    case Opt_dump_tokens:
    default:
        return new DumpToken();
    }
}
