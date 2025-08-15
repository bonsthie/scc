#ifndef SCC_OPTION_OPTIONTABLE_H
#define SCC_OPTION_OPTIONTABLE_H

#include "scc/Option/Args.h"
#include <memory>
#include <optional>
#include <ostream>
#include <span>

namespace scc {
namespace opt {

enum class OptKind { Flag, Joined, Separate, JoinedOrSeparate, Equal };
enum class ValType { None, Str, StrList };

using ArgvIt = std::vector<const char *>::const_iterator;

struct OptionSpec {
    int         key;      // Opt_target, Opt_inc, Opt_o
    const char *spelling; // "-I", "-D", "-O", "--std=", "-Wl,", ...
    OptKind     kind;
    ValType     vtype;
    const char *help;
    bool        hidde = false; // his the option display in --help. use for file extension
};

class OptionTable {
    virtual std::span<const OptionSpec> specs() const = 0;

  public:
    ~OptionTable() = default;

    ArgsList *parseArgs(const std::vector<const char *> &argv);

    void printOpt(std::ostream &O);

  private:
    std::string printHasOption(OptKind kind);

    std::unique_ptr<Arg> nextArg(ArgvIt &it, ArgvIt end);
};

} // namespace opt
} // namespace scc

#endif // SCC_OPTION_OPTIONTABLE_H
