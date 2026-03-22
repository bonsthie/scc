#ifndef SCC_AST_QUALTYPE_H
#define SCC_AST_QUALTYPE_H

#include "scc/AST/Qualifiers.h"
#include "scc/AST/Type.h"

namespace scc {

class QualType {
    Type      *Ty = nullptr;
    Qualifiers Quals;

  public:
    QualType() = default;
    QualType(Type *Ty, Qualifiers Quals = {}) : Ty(Ty), Quals(Quals) {}

    void              setType(Type *T) { Ty = T; }
    void              setQualifiers(Qualifiers Q) { Quals = Q; }
    Type             *getType() const { return Ty; }
    const Qualifiers &getQualifiers() const { return Quals; }
    bool              isNull() const { return Ty == nullptr; }

    bool isConstQualified() const { return Quals.IsConst; }
    bool isRestrictQualified() const { return Quals.IsRestrict; }
    bool isVolatileQualified() const { return Quals.IsVolatile; }

    void addConst() { Quals.IsConst = true; }
    void addRestrict() { Quals.IsRestrict = true; }
    void addVolatile() { Quals.IsVolatile = true; }
    void addQualifiers(const Qualifiers &Q) { Quals.merge(Q); }

    TypeKind kind() const {
        if (!Ty)
            return TypeKind::Uninitialized;
        return Ty->kind();
    }

    QualType desugarOnce() const;
    bool     operator==(const QualType &Other) const;
    bool     operator!=(const QualType &Other) const { return !(*this == Other); }
};

} // namespace scc

#endif // SCC_AST_QUALTYPE_H
