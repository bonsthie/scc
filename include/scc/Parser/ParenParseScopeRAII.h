#ifndef SCC_PARSER_PARENPARSESCOPERAII_H
#define SCC_PARSER_PARENPARSESCOPERAII_H

#include "scc/Parser/Parser.h"

namespace scc {

class ParenParseScopeRAII {
    Parser &P;
    bool    OldInParen;

  public:
    explicit ParenParseScopeRAII(Parser &P) : P(P), OldInParen(P.InParen) { P.InParen = true; }

    ~ParenParseScopeRAII() { P.InParen = OldInParen; }
};

} // namespace scc

#endif // SCC_PARSER_PARENPARSESCOPERAII_H
