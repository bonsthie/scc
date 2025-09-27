#ifndef SCCLEXTOKEN_H
#define SCCLEXTOKEN_H

namespace scc {

namespace tok {

enum TokenKind {
#define TOK(X) X,
#include "TokenKinds.def"

	NUMBER_OF_TOKENS
};

} // namespace tok

class Token {};

} // namespace scc

#endif // SCCLEXTOKEN_H
