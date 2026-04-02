#ifndef SCC_AST_ARRAYTYPE_H
#define SCC_AST_ARRAYTYPE_H

#include <cstdint>

#include "scc/AST/Type.h"

namespace scc {

class ArrayType : public Type {
    Type    *BaseTy = nullptr;
    uint64_t Size = 0;

  public:
    ArrayType() : Type(TypeKind::Array) {}
    ArrayType(Type *BaseTy, uint64_t Size) : Type(TypeKind::Array), BaseTy(BaseTy), Size(Size) {}

    void     setBaseType(Type *T) { BaseTy = T; }
    Type    *getBaseType() const { return BaseTy; }
    void     setSize(uint64_t N) { Size = N; }
    uint64_t getSize() const { return Size; }
};

} // namespace scc

#endif // SCC_AST_ARRAYTYPE_H
