#ifndef SCC_AST_POINTERTYPE_H
#define SCC_AST_POINTERTYPE_H

#include "scc/AST/Type.h"

namespace scc {

class PointerType : public Type {
    Type *PointeeTy = nullptr;

  public:
    PointerType() : Type(TypeKind::Pointer) {}
    explicit PointerType(Type *PointeeTy) : Type(TypeKind::Pointer), PointeeTy(PointeeTy) {}

    void  setPointeeType(Type *T) { PointeeTy = T; }
    Type *getPointeeType() const { return PointeeTy; }
};

} // namespace scc

#endif // SCC_AST_POINTERTYPE_H
