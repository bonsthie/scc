#ifndef SCC_AST_TYPEDEFTYPE_H
#define SCC_AST_TYPEDEFTYPE_H

#include <cassert>
#include <string_view>

#include "scc/AST/QualType.h"

namespace scc {

class TypedefType : public Type {
    std::string_view Name;
    Type            *UnderlyingTy = nullptr;
    Qualifiers       UnderlyingQuals;

  public:
    TypedefType() : Type(TypeKind::Typedef) {}
    TypedefType(std::string_view Name, Type *UnderlyingTy, Qualifiers Quals = {})
        : Type(TypeKind::Typedef),
          Name(Name),
          UnderlyingTy(UnderlyingTy),
          UnderlyingQuals(Quals) {
        assert(!Name.empty() && "scc::TypedefType Name can't be empty");
        assert(UnderlyingTy && "scc::TypedefType Type can't be null");
    }

    std::string_view getName() const { return Name; }

    QualType getUnderlyingType() const;
};

} // namespace scc

#endif // SCC_AST_TYPEDEFTYPE_H
