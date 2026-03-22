#include "scc/AST/Type.h"
#include "scc/AST/QualType.h"

using namespace scc;

QualType TypedefType::getUnderlyingType() const {
    return QualType(UnderlyingTy, UnderlyingQuals);
}
