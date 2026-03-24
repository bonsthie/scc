
#ifndef SCC_AST_BUILTINTYPE_H
#define SCC_AST_BUILTINTYPE_H

#include "scc/AST/Type.h"
namespace scc {

enum BuiltinTypeKind {
#define TYPE_KEYWORD(x) TY##x,
#include "scc/Token/TokenKinds.def"

};

class BuiltinType : public Type {
    BuiltinTypeKind BKind;

  public:
    BuiltinType(BuiltinTypeKind BKind) : Type(TypeKind::Builtin), BKind(BKind) {}

    BuiltinTypeKind getBuiltinKind() const { return BKind; }
};

} // namespace scc
#endif // SCC_AST_BUILTINTYPE_H
