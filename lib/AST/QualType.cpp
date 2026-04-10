#include "scc/AST/QualType.h"
#include "scc/AST/TypedefType.h"

using namespace scc;

QualType QualType::desugarOnce() const {
    if (!Ty || !Ty->isTypedefType())
        return *this;

    auto    *TT = static_cast<TypedefType *>(Ty);
    QualType Under = TT->getUnderlyingType();
    Under.addQualifiers(Quals);
    return Under;
}
