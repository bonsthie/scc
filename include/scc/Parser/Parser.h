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
};

} // namespace scc

#endif // SCC_PARSER_PARSER_H
