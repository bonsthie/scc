#ifndef SCC_LEX_DECODECHAR_H
#define SCC_LEX_DECODECHAR_H

#include "scc/Lex/SizedChar.h"
namespace scc {

SizedChar decode_logical_char(const char *Ptr, const char *End);

} // namespace scc

#endif // SCC_LEX_DECODECHAR_H
