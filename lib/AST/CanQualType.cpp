#include "scc/AST/CanQualType.h"

using namespace scc;

CanQualType::CanQualType(QualType QT) { *this = create(QT); }

bool CanQualType::isNull() const { return Canonical.isNull(); }

const QualType &CanQualType::asQualType() const { return Canonical; }

QualType CanQualType::desugarOnce() const { return Canonical.desugarOnce(); }

bool CanQualType::operator==(const CanQualType &Other) const {
    return Canonical == Other.Canonical;
}

bool CanQualType::operator!=(const CanQualType &Other) const { return !(*this == Other); }

CanQualType CanQualType::create(QualType QT) {
    while (QT.kind() == TypeKind::Typedef)
        QT = QT.desugarOnce();

    CanQualType Result;
    Result.Canonical = QT;
    return Result;
}
