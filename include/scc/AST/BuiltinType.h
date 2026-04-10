
#ifndef SCC_AST_BUILTINTYPE_H
#define SCC_AST_BUILTINTYPE_H

#include "scc/AST/Type.h"
#include "scc/Token/Token.h"
namespace scc {

enum BuiltinTypeKind {
    TYunspecified = tok::not_init,
#define TYPE_KEYWORD(x) TY##x = tok::t_##x,
#include "scc/Token/TokenKinds.def"
};

class BuiltinType : public Type {
    BuiltinTypeKind BKind;

  public:
    constexpr BuiltinType(BuiltinTypeKind BKind) : Type(TypeKind::Builtin), BKind(BKind) {}

    constexpr BuiltinTypeKind getBuiltinKind() const { return BKind; }
};

} // namespace scc
#endif // SCC_AST_BUILTINTYPE_H
