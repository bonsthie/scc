#ifndef SCC_OPTION_OPTIONTABLE_H
#define SCC_OPTION_OPTIONTABLE_H

#include "scc/ADT/vector.h"
#include "scc/Error/ErrorManager.h"
#include "scc/Option/Args.h"
#include <memory>
#include <optional>
#include <ostream>
#include <span>

namespace scc {

enum class OptKind { Flag, Joined, Separate, JoinedOrSeparate, Equal };
enum class ValType { None, Str, StrList };

using ArgvIt = scc::Vector<const char *>::const_iterator;

struct OptionSpec {
    int         Key;      // Opt_target, Opt_inc, Opt_o
    const char *Spelling; // "-I", "-D", "-O", "--std=", "-Wl,", ...
    OptKind     Kind;
    ValType     Vtype;
    const char *Help;
    bool        Hidde = false; // his the option display in --help. use for file extension
};

class OptionTable {
    virtual std::span<const OptionSpec> specs() const = 0;
    ErrorManager                       &EM;

  public:
    OptionTable(ErrorManager &EM) : EM(EM) {}
    ~OptionTable() = default;

    ArgsList *parseArgs(const scc::Vector<const char *> &Argv);

    void printOpt(std::ostream &O);

  private:
    std::string printHasOption(OptKind Kind);

    std::unique_ptr<Arg> nextArg(ArgvIt &It, ArgvIt End, std::string *MatchedSpelling = nullptr);
};

} // namespace scc

#endif // SCC_OPTION_OPTIONTABLE_H
