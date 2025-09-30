#ifndef SCC_TOKEN_TOKENSTREAM_H
#define SCC_TOKEN_TOKENSTREAM_H

#include "Token.h"

namespace scc {

// Base Class use to implement every token stream use by the PreProcesseur
// main one FileLexer and DefineTokenStream
class TokenStream {
  public:
    virtual ~TokenStream() = default;
    virtual Token next() = 0;
};

} // namespace scc

#endif // SCC_TOKEN_TOKENSTREAM_H
