#ifndef SCC_FRONTEND_COMPILERINSTANCESETTINGS_H
#define SCC_FRONTEND_COMPILERINSTANCESETTINGS_H

#include "scc/Frontend/LangOpt.h"

namespace scc {

// Placeholder for frontend configuration.
// Real examples of future frontend options:
// size_t MaxIncludeDepth = 64;
struct CompilerInstanceSettings {
    LangOpt LangOptions{};
};

} // namespace scc

#endif // SCC_FRONTEND_COMPILERINSTANCESETTINGS_H
