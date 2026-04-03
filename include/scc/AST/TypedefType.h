#ifndef SCC_AST_TYPEDEFTYPE_H
#define SCC_AST_TYPEDEFTYPE_H

#include <cassert>

#include "scc/AST/Decl.h"

namespace scc {

class TypedefType : public Type {
    TypedefDecl *Decl = nullptr;

  public:
    TypedefType() : Type(TypeKind::Typedef) {}
    explicit TypedefType(TypedefDecl *Decl) : Type(TypeKind::Typedef), Decl(Decl) {
        assert(Decl && "scc::TypedefType Decl can't be null");
    }

    TypedefDecl       *getDecl() { return Decl; }
    const TypedefDecl *getDecl() const { return Decl; }
    void               setDecl(TypedefDecl *NewDecl) { Decl = NewDecl; }

    std::string_view getName() const {
        assert(Decl && "scc::TypedefType Decl can't be null");
        return *Decl->getName();
    }

    QualType getUnderlyingType() const;
};

} // namespace scc

#endif // SCC_AST_TYPEDEFTYPE_H
