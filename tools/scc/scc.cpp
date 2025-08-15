#include "scc/Option/OptionTable.h"
#include <cstring>
#include <iostream>
#include <memory>

using namespace scc;
using namespace scc::opt;

namespace scc {
enum SccOptionIndex { Opt_none, Opt_test, Opt_oui, Opt_L };

class SccOptionTable : public OptionTable {

    static constexpr OptionSpec opt[] = {
        {Opt_test, "-test", OptKind::Flag, ValType::None, "this is a test", false},
        {Opt_oui, "--oui=", OptKind::Equal, ValType::Str, "oui oui baguette", false},
        {Opt_L, "-L", OptKind::Separate, ValType::StrList, "L is for losser", false},
    };

    virtual std::span<const OptionSpec> specs() const { return opt; }
};

} // namespace scc

int cc1(int argc, char **argv, char **env) { return 1; }

int main(int argc, char **argv, char **env) {
    if (argc >= 2 && strcmp(argv[1], "-cc1") == 0)
        return cc1(argc, argv, env);

    // FileManager FM;

    SccOptionTable Opt;

    std::unique_ptr<ArgsList> Args(Opt.parseArgs(std::vector<const char *>(argv + 1, argv + argc)));
    if (!Args)
        return 10;

    // Opt.printOpt(std::cout);

    if (auto a = Args->getArg(Opt_oui))
        std::cout << *a->getValue() << std::endl;

    if (auto a = Args->getArg(Opt_L)) {
        auto strs = a->getValuesList();
        if (strs) {
            std::cout << "opt value : ";
            for (auto str : *strs) {
                std::cout << str << " ";
            }
            std::cout << std::endl;
        } else {
            std::cout << "opt L no value" << std::endl;
        }
    }

	Opt.printOpt(std::cout);

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
