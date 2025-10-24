#include "scc/Error/Error.h"
#include "scc/Error/ErrorManager.h"
#include "scc/FileManager/FileFinder.h"
#include "scc/FileManager/FileManager.h"
#include "scc/Lex/FileLexer.h"
#include "scc/Option/OptionTable.h"
#include "scc/Token/Token.h"
#include <cstring>
#include <iostream>
#include <memory>

using namespace scc;

namespace scc {
enum SccOptionIndex { Opt_none, Opt_test, Opt_oui, Opt_L, Opt_I };

class SccOptionTable : public OptionTable {

    static constexpr OptionSpec opt[] = {
        {Opt_test, "-test", OptKind::Flag, ValType::None, "this is a test", false},
        {Opt_oui, "--oui=", OptKind::Equal, ValType::Str, "oui oui baguette", false},
        {Opt_L, "-L", OptKind::Separate, ValType::StrList, "L is for losser", false},
        {Opt_I, "-I", OptKind::JoinedOrSeparate, ValType::StrList, "system include folder", false},
    };

    virtual std::span<const OptionSpec> specs() const { return opt; }

  public:
    SccOptionTable(ErrorManager &EM) : OptionTable(EM) {}
};

} // namespace scc

int cc1(int argc, char **argv, char **env) { return 1; }

int main(int argc, char **argv, char **env) {

    if (argc >= 2 && strcmp(argv[1], "-cc1") == 0)
        return cc1(argc, argv, env);

    // FileManager FM;

    ErrorManager   EM;
    SccOptionTable Opt(EM);

    std::unique_ptr<ArgsList> Args(Opt.parseArgs(std::vector<const char *>(argv + 1, argv + argc)));
    if (!Args)
        return 10;
    EM.emit();
    // return 1;

    auto        Inc = Args->getArg(Opt_I);
    FileFinder  FF(Inc ? Inc->getValuesList() : std::vector<std::string>{});
    FileManager FM(FF, EM);

    File *F = FM.getFile("LexerTest");
    if (!F) {
        std::cerr << "File not found\n";
        return 1;
    }

    FileLexer FL(*F);

    Token CurTok;
    while (FL.next(CurTok) == false) {
        CurTok.print(std::cout);
		CurTok.flush();
    }
	CurTok.print(std::cout);
    if (CurTok.is(tok::comment)) {
        std::cout << "unfinish comment";
    }

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
