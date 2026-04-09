#ifndef SCC_FRONTED_CC1ARGS_H
#define SCC_FRONTED_CC1ARGS_H

#include "scc/Option/OptionTable.h"

namespace scc {
enum SccCC1OptionIndex {
    Opt_none,
    Opt_test,
    Opt_std,
    Opt_oui,
    Opt_L,
    Opt_I,
    Opt_cc1,
    Opt_dump_tokens,
    Opt_dump_raw_tokens,
    Opt_dump_ast
};

class SccOptionTable : public OptionTable {

    static constexpr OptionSpec opt[] = {
        {Opt_test, "-test", OptKind::Flag, ValType::None, "this is a test", false},
        {Opt_oui, "--oui=", OptKind::Equal, ValType::Str, "oui oui baguette", false},
        {Opt_std, "--std=", OptKind::Equal, ValType::Str, "set the default c version", false},
        {Opt_L, "-L", OptKind::Separate, ValType::StrList, "L is for losser", false},
        {Opt_I, "-I", OptKind::JoinedOrSeparate, ValType::StrList, "system include folder", false},
        {Opt_dump_tokens, "-dump-tokens", OptKind::Flag, ValType::None, "dump tokens", false},
        {Opt_dump_raw_tokens, "-dump-raw-tokens", OptKind::Flag, ValType::None, "dump raw tokens",
         false},
        {Opt_dump_ast, "-dump-ast", OptKind::Flag, ValType::None, "dump ast", false},
    };

    virtual std::span<const OptionSpec> specs() const { return opt; }

  public:
    SccOptionTable(ErrorManager &EM) : OptionTable(EM) {}
};

} // namespace scc

#endif // SCC_FRONTED_CC1ARGS_H
