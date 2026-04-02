#include "scc/AST/TypedefType.h"

using namespace scc;

QualType TypedefType::getUnderlyingType() const { return QualType(UnderlyingTy, UnderlyingQuals); }
