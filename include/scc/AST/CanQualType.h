#ifndef SCC_AST_CANQUALTYPE_H
#define SCC_AST_CANQUALTYPE_H

#include "scc/AST/QualType.h"

namespace scc {

// Canonical Qualifier Type
class CanQualType {
    QualType Canonical;

  public:
    constexpr CanQualType() = default;
    explicit constexpr CanQualType(QualType QT) : Canonical(QT) {}

    static CanQualType create(QualType QT);

    constexpr bool            isNull() const { return Canonical.isNull(); }
    constexpr const QualType &asQualType() const { return Canonical; }
    QualType                  desugarOnce() const { return Canonical.desugarOnce(); }
    constexpr bool            operator==(const CanQualType &Other) const {
        return Canonical == Other.Canonical;
    }
    constexpr bool operator!=(const CanQualType &Other) const { return !(*this == Other); }
};

} // namespace scc

#endif // SCC_AST_CANQUALTYPE_H
