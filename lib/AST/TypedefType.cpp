#include "scc/AST/TypedefType.h"

using namespace scc;

QualType TypedefType::getUnderlyingType() const {
    return Decl ? Decl->getUnderlyingType() : QualType();
}
