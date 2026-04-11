#ifndef SCC_FRONTEND_LANGOPT_H
#define SCC_FRONTEND_LANGOPT_H

#include "scc/Error/ErrorManager.h"
#include "scc/Option/Args.h"
#include "scc/Support/Attributes.h"
#include <cstdint>
#include <expected>

namespace scc {

enum class LangVersion : size_t {
    C89,
    GNU89,
    C99,
    GNU99,

    SIZE,
    LAST = C99,
    UNKONW,
};

struct LangOpt {
#define ALIAS_FLAG(Enum, FlagType, Kind, VType, Desc, Hidden)
#define FLAG(Enum, FlagType, Kind, VType, Desc, Hidden)
#define WARN_FLAG(Enum, FlagType, Desc, Hidden)                                                    \
    SCC_PREFERRED_TYPE(bool)                                                                       \
    uint64_t Enum##Warning : 1 = false;

#define F_FLAG(Enum, FlagType, Desc, Hidden)                                                       \
    SCC_PREFERRED_TYPE(bool)                                                                       \
    uint64_t Enum##Enable : 1 = false;

#include "scc/Frontend/CC1Args.def"
};

class LangOptBuilder {
    ArgsList     &Args;
    ErrorManager &EM;
    LangVersion   LangDefault;

  public:
    LangOptBuilder(ArgsList &Args, ErrorManager &EM, LangVersion LangDefault = LangVersion::LAST)
        : Args(Args),
          EM(EM),
          LangDefault(LangDefault) {}

    std::expected<LangOpt, err::Code> build();

  private:
    LangVersion getCVersion();
    void        addWarnindAndFeatureFlags(LangOpt &Opts);
};

} // namespace scc

#endif // SCC_FRONTEND_LANGOPT_H
