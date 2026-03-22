#ifndef SCC_AST_CANQUALTYPE_H
#define SCC_AST_CANQUALTYPE_H

#include "scc/AST/QualType.h"

namespace scc {

// Canonical Qualifier Type
class CanQualType {
    QualType Canonical;

  public:
    CanQualType() = default;
    explicit CanQualType(QualType QT);

    static CanQualType create(QualType QT);

    bool            isNull() const;
    const QualType &asQualType() const;
    QualType        desugarOnce() const;
    bool            operator==(const CanQualType &Other) const;
    bool            operator!=(const CanQualType &Other) const;
};

} // namespace scc

#endif // SCC_AST_CANQUALTYPE_H
