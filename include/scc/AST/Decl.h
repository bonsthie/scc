#ifndef SCC_AST_DECL_H
#define SCC_AST_DECL_H

#include <iostream>
#include <optional>
#include <ostream>
#include <string_view>
#include <utility>

#include "scc/ADT/Span.h"
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

    virtual void print(std::ostream &o = std::cout) const;
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

    void print(std::ostream &o = std::cout) const override;
};

class TypeDecl : public NamedDecl {
  public:
    using NameType = NamedDecl::NameType;

    TypeDecl(DeclKind Kind, NameType Name) : NamedDecl(Kind, Name) {}

    void print(std::ostream &o = std::cout) const override;
};

class ValueDecl : public NamedDecl {
    QualType Ty;

  public:
    using NameType = NamedDecl::NameType;

    ValueDecl(DeclKind Kind, NameType Name, QualType Ty) : NamedDecl(Kind, Name), Ty(Ty) {}

    QualType getType() const { return Ty; }
    void     setType(QualType NewTy) { Ty = NewTy; }

    void print(std::ostream &o = std::cout) const override;
};

class TypedefDecl : public TypeDecl {
    QualType UnderlyingType;

  public:
    TypedefDecl(NameType Name, QualType UnderlyingType)
        : TypeDecl(Typedef, Name),
          UnderlyingType(UnderlyingType) {}

    QualType getUnderlyingType() const { return UnderlyingType; }
    void     setUnderlyingType(QualType NewUnderlyingType) { UnderlyingType = NewUnderlyingType; }

    void print(std::ostream &o = std::cout) const override;
};

class VarDecl : public ValueDecl {
  public:
    using NameType = ValueDecl::NameType;

    VarDecl(NameType Name, QualType Ty) : ValueDecl(Var, Name, Ty) {}

    void print(std::ostream &o = std::cout) const override;
};

using DeclList = scc::Span<Decl *>;

} // namespace scc

#endif // SCC_AST_DECL_H
