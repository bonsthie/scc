#ifndef SCC_AST_TAGDECL_H
#define SCC_AST_TAGDECL_H

#include "scc/ADT/Span.h"
#include "scc/AST/Decl.h"
#include "scc/AST/QualType.h"
#include <optional>
#include <string_view>

namespace scc {

enum TagDeclKind { Enum, Struct, Union };

class TagDecl : public Decl {
  public:
    using NameType = std::optional<std::string_view>;

  private:
    TagDeclKind TKind;
    NameType    Name;

  public:
    TagDecl(TagDeclKind TKind, NameType Name) : Decl(Tag), TKind(TKind), Name(Name) {}

    TagDeclKind     getTagDeclKind() const { return TKind; }
    const NameType &getName() const { return Name; }
    void            setName(NameType NewName) { Name = NewName; }

    bool isAnonymous() const { return !Name.has_value(); }
    bool isEnum() const { return TKind == Enum; }
    bool isStruct() const { return TKind == Struct; }
    bool isUnion() const { return TKind == Union; }
    bool isRecord() const { return isStruct() || isUnion(); }
};

class RecordFieldDecl : public Decl {
  public:
    using NameType = TagDecl::NameType;

  private:
    QualType Ty;
    NameType Name;

  public:
    RecordFieldDecl(QualType Ty, NameType Name = std::nullopt)
        : Decl(RecordField),
          Ty(Ty),
          Name(Name) {}

    QualType        getType() const { return Ty; }
    const NameType &getName() const { return Name; }
    void            setType(QualType NewTy) { Ty = NewTy; }
    void            setName(NameType NewName) { Name = NewName; }
    bool            isAnonymous() const { return !Name.has_value(); }
};

class RecordDecl : public TagDecl {
  public:
    using FieldType = scc::Span<RecordFieldDecl>;
    using ConstFieldType = scc::Span<const RecordFieldDecl>;

  private:
    RecordFieldDecl *FieldsData = nullptr;
    size_t           FieldCount = 0;

  public:
    RecordDecl(TagDeclKind TKind, NameType Name, RecordFieldDecl *FieldsData, size_t FieldCount)
        : TagDecl(TKind, Name),
          FieldsData(FieldsData),
          FieldCount(FieldCount) {}

    explicit RecordDecl(TagDeclKind TKind, FieldType Fields, NameType Name = std::nullopt)
        : RecordDecl(TKind, Name, Fields.data(), Fields.size()) {}

    FieldType      getFields() { return {FieldsData, FieldCount}; }
    ConstFieldType getFields() const { return {FieldsData, FieldCount}; }

    void setFields(RecordFieldDecl *Data, size_t Count) {
        FieldsData = Data;
        FieldCount = Count;
    }

    void setFields(FieldType Fields) {
        FieldsData = Fields.data();
        FieldCount = Fields.size();
    }

    bool   empty() const { return FieldCount == 0; }
    size_t size() const { return FieldCount; }
};

class EnumFieldDecl : public Decl {
  public:
    using NameType = TagDecl::NameType;

  private:
    NameType Name;

  public:
    explicit EnumFieldDecl(NameType Name) : Decl(EnumField), Name(Name) {}

    const NameType &getName() const { return Name; }
    void            setName(NameType NewName) { Name = NewName; }
    bool            isAnonymous() const { return !Name.has_value(); }
};

class EnumDecl : public TagDecl {
  public:
    using FieldType = scc::Span<EnumFieldDecl>;
    using ConstFieldType = scc::Span<const EnumFieldDecl>;

  private:
    EnumFieldDecl *FieldsData = nullptr;
    size_t         FieldCount = 0;

  public:
    EnumDecl(TagDeclKind TKind, NameType Name, EnumFieldDecl *FieldsData, size_t FieldCount)
        : TagDecl(TKind, Name),
          FieldsData(FieldsData),
          FieldCount(FieldCount) {}

    explicit EnumDecl(TagDeclKind TKind, FieldType Fields, NameType Name = std::nullopt)
        : EnumDecl(TKind, Name, Fields.data(), Fields.size()) {}

    FieldType      getFields() { return {FieldsData, FieldCount}; }
    ConstFieldType getFields() const { return {FieldsData, FieldCount}; }

    void setFields(EnumFieldDecl *Data, size_t Count) {
        FieldsData = Data;
        FieldCount = Count;
    }

    void setFields(FieldType Fields) {
        FieldsData = Fields.data();
        FieldCount = Fields.size();
    }

    bool   empty() const { return FieldCount == 0; }
    size_t size() const { return FieldCount; }
};

} // namespace scc

#endif // SCC_AST_TAGDECL_H
