#ifndef SCC_PARSER_PARSER_H
#define SCC_PARSER_PARSER_H

#include "scc/PreProcessor/PreProcessor.h"
#include "scc/Sema/Sema.h"

namespace scc {

class Parser {
    PreProcessor &PP;
    Sema         &Action;

  public:
    Parser(PreProcessor &PP, Sema &Action) : PP(PP), Action(Action) {}

    PreProcessor &getPreprocessor() { return PP; }
    const PreProcessor &getPreprocessor() const { return PP; }

    Sema &getSema() { return Action; }
    const Sema &getSema() const { return Action; }
};

} // namespace scc

#endif // SCC_PARSER_PARSER_H
