#include "scc/ADT/vector.h"
#include "scc/Colors/Colors.h"
#include "scc/Error/Error.h"
#include "scc/Error/ErrorManager.h"
#include "scc/FileManager/FileFinder.h"
#include "scc/FileManager/FileManager.h"
#include "scc/Frontend/CC1Args.h"
#include "scc/Frontend/CompilerBuilder.h"
#include "scc/Frontend/CompilerInstance.h"
#include "scc/Frontend/FrontendErrorManager.h"
#include "scc/Frontend/LangOpt.h"
#include "scc/PreProcessor/PreProcessor.h"
#include "scc/String/StringInterner.h"
#include "scc/Token/Token.h"
#include <cstring>
#include <iostream>
#include <memory>

using namespace scc;

namespace {

void configureColor(const ArgsList &Args) { Color::setDisable(!Args.hasArg(Opt_disable_colors)); }

} // namespace

bool cc1(int argc, char **argv, char **) {
    FrontendErrorManager EM;
    SccOptionTable Opt(EM);

    std::unique_ptr<ArgsList> Args(Opt.parseArgs(scc::vector<const char *>(argv + 2, argv + argc)));
    if (Args)
        configureColor(*Args);
    if (EM.emit() || !Args)
        return 1;

    if (Args->hasArg(Opt_help)) {
        Opt.printOpt(std::cout);
        return false;
    }

    BumpAllocator  BumpAlloca;
    StringInterner SI(BumpAlloca);

    auto        Inc = Args->getArg(Opt_I);
    FileFinder  FF(Inc ? Inc->getValuesList() : scc::vector<std::string>{});
    FileManager FM(FF, EM);

    CompilerBuilder Builder(FM, EM, *Args);
    auto            Files = Args->getFiles();

    std::unique_ptr<CompilerInstance> CI;

    if (!Files.empty()) {
        std::string F(Files.back());
        if (Files.size() != 1) {
            EM.report(err::warning)
                .msg("multiple input files specified; using ")
                .quote(F)
                .msg(" and ignoring previous inputs");
            if (EM.emit())
                return 1;
        }
        CI.reset(Builder.create(F));
    } else
        CI.reset(Builder.create());

    if (CI == nullptr) {
        return EM.emit();
    }

    return CI->Execute();
}

int main(int argc, char **argv, char **env) {

    if (argc >= 2 && strcmp(argv[1], "-cc1") == 0)
        return cc1(argc, argv, env);

    FrontendErrorManager EM;
    SccOptionTable Opt(EM);

    std::unique_ptr<ArgsList> Args(Opt.parseArgs(scc::vector<const char *>(argv + 1, argv + argc)));
    if (Args)
        configureColor(*Args);
    if (!Args)
        return 10;
    EM.emit();
    // return 1;

    auto        Inc = Args->getArg(Opt_I);
    FileFinder  FF(Inc ? Inc->getValuesList() : scc::vector<std::string>{});
    FileManager FM(FF, EM);

    File *F = FM.getFile("LexerTest.c");
    if (!F) {
        std::cerr << "File not found\n";
        return 1;
    }

    BumpAllocator  BumpAlloca;
    StringInterner SI(BumpAlloca);

    LangOptBuilder LangOptB(*Args, EM);
    auto           LangOptions = LangOptB.build();
    if (!LangOptions)
        return EM.emit();

    PreProcessor PP(*F, EM, FM, SI, *LangOptions);

    Token CurTok;
    do {
        CurTok.flush();
        PP.nextRaw(CurTok);
        std::cout << CurTok << std::endl;
        if (EM.size())
            EM.emit();
    } while (!CurTok.is(tok::eof));

    // // Opt.printOpt(std::cout);
    // if (auto a = Args->getArg(Opt_oui))
    //     std::cout << "--oui=" << a->getValue() << std::endl;
    //
    // if (auto a = Args->getArg(Opt_L)) {
    //     auto strs = a->getValuesList();
    //     std::cout << "opt value : ";
    //     for (auto str : strs) {
    //         std::cout << str << " ";
    //     }
    //     std::cout << std::endl;
    // } else {
    //     std::cout << "opt L no value" << std::endl;
    // }
    //
    // Opt.printOpt(std::cout);
    //
    // std::cout << "\n === warning === \n";
    //
    // EM.report(err::warning).msg("j'ai les crampte").msg(" de bzh");
    // EM.report(err::error).msg("rhaaaaaaaaaaaa");
    //
    // EM.emit();
    // WarningSystem WS;
    // if (WarningSystem.setup(*Args))
    // return ;

    // Driver TheDriver(FS, WS);

    // std::unique_ptr<Compilation> C =  Driver.createCompilation(*Args);
    // if (!C)
    // return;

    // DriverStatus status = Driver.run(*C);

    // handleerror
    return 0;
}
