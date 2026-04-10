#include "scc/AST/CanQualType.h"

using namespace scc;

CanQualType CanQualType::create(QualType QT) {
    while (QT.kind() == TypeKind::Typedef)
        QT = QT.desugarOnce();

    return CanQualType(QT);
}
