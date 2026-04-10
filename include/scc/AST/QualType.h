#ifndef SCC_AST_QUALTYPE_H
#define SCC_AST_QUALTYPE_H

#include "scc/AST/Qualifiers.h"
#include "scc/AST/Type.h"

namespace scc {

class QualType {
    Type      *Ty = nullptr;
    Qualifiers Quals;

  public:
    constexpr QualType() = default;
    constexpr QualType(Type *Ty, Qualifiers Quals = {}) : Ty(Ty), Quals(Quals) {}

    constexpr void              setType(Type *T) { Ty = T; }
    constexpr void              setQualifiers(Qualifiers Q) { Quals = Q; }
    constexpr Type             *getType() const { return Ty; }
    constexpr const Qualifiers &getQualifiers() const { return Quals; }
    constexpr bool              isNull() const { return Ty == nullptr; }

    constexpr bool isConstQualified() const { return Quals.IsConst; }
    constexpr bool isRestrictQualified() const { return Quals.IsRestrict; }
    constexpr bool isVolatileQualified() const { return Quals.IsVolatile; }

    constexpr void addConst() { Quals.IsConst = true; }
    constexpr void addRestrict() { Quals.IsRestrict = true; }
    constexpr void addVolatile() { Quals.IsVolatile = true; }
    constexpr void addQualifiers(const Qualifiers &Q) { Quals.merge(Q); }

    constexpr TypeKind kind() const {
        if (!Ty)
            return TypeKind::Uninitialized;
        return Ty->kind();
    }

    QualType desugarOnce() const;
    constexpr bool operator==(const QualType &Other) const {
        return Ty == Other.Ty && Quals == Other.Quals;
    }
    constexpr bool operator!=(const QualType &Other) const { return !(*this == Other); }
};

} // namespace scc

#endif // SCC_AST_QUALTYPE_H
