#ifndef SCC_COMPILERINSTANCE_H
#define SCC_COMPILERINSTANCE_H

namespace scc {

class CompilerInstance {
    virtual bool execute() = 0;
};

} // namespace scc

#endif // SCC_COMPILERINSTANCE_H
