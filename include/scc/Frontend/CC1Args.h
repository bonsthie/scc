#ifndef SCC_FRONTEND_CC1ARGS_H
#define SCC_FRONTEND_CC1ARGS_H

#include "scc/Option/OptionTable.h"

namespace scc {

enum SccCC1OptionIndex {
    Opt_none,
#define OPT_ENUM
#include "scc/Frontend/CC1Args.def"
};

class SccOptionTable : public OptionTable {

    static constexpr OptionSpec opt[] = {
#include "scc/Frontend/CC1Args.def"
    };

    std::span<const OptionSpec> specs() const override { return opt; }

  public:
    SccOptionTable(ErrorManager &EM) : OptionTable(EM) {}
};

} // namespace scc

#endif // SCC_FRONTEND_CC1ARGS_H
