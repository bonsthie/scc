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

    // Lexer Option
    SCC_PREFERRED_TYPE(bool)
    uint64_t TrigraphEnable : 1 = false;

    // Parsing / Sema Option

    // -std=c89 option so that by default if not specify the type is int
    // ex : foo() { return 1; } or typedef foo;
    SCC_PREFERRED_TYPE(bool)
    uint64_t TypeDefaultToINT : 1 = false;
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
};

} // namespace scc

#endif // SCC_FRONTEND_LANGOPT_H
