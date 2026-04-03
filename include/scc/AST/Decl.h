#ifndef SCC_AST_DECL_H
#define SCC_AST_DECL_H

#include <optional>
#include <string_view>

#include "scc/AST/QualType.h"

namespace scc {

enum DeclKind {
    None,
    Type,
    Tag,
    Typedef,
    Var,
    RecordField,
    EnumField,
};

class Decl {
    DeclKind Kind;

  public:
    Decl(DeclKind Kind) : Kind(Kind) {}

    DeclKind kind() const { return Kind; }
};

class NamedDecl : public Decl {
  public:
    using NameType = std::optional<std::string_view>;

  private:
    NameType Name;

  public:
    NamedDecl(DeclKind Kind, NameType Name) : Decl(Kind), Name(Name) {}

    const NameType &getName() const { return Name; }
    void            setName(NameType NewName) { Name = NewName; }
    bool            isAnonymous() const { return !Name.has_value(); }
};

class TypeDecl : public NamedDecl {
  public:
    using NameType = NamedDecl::NameType;

    TypeDecl(DeclKind Kind, NameType Name) : NamedDecl(Kind, Name) {}
};

class ValueDecl : public NamedDecl {
    QualType Ty;

  public:
    using NameType = NamedDecl::NameType;

    ValueDecl(DeclKind Kind, NameType Name, QualType Ty) : NamedDecl(Kind, Name), Ty(Ty) {}

    QualType getType() const { return Ty; }
    void     setType(QualType NewTy) { Ty = NewTy; }
};

class TypedefDecl : public TypeDecl {
    QualType UnderlyingType;

  public:
    TypedefDecl(NameType Name, QualType UnderlyingType)
        : TypeDecl(Typedef, Name),
          UnderlyingType(UnderlyingType) {}

    QualType getUnderlyingType() const { return UnderlyingType; }
    void     setUnderlyingType(QualType NewUnderlyingType) { UnderlyingType = NewUnderlyingType; }
};

class VarDecl : public ValueDecl {
  public:
    using NameType = ValueDecl::NameType;

    VarDecl(NameType Name, QualType Ty) : ValueDecl(Var, Name, Ty) {}
};

} // namespace scc

#endif // SCC_AST_DECL_H
