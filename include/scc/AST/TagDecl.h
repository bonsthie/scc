#ifndef SCC_AST_TAGDECL_H
#define SCC_AST_TAGDECL_H

#include "scc/ADT/Span.h"
#include "scc/AST/Decl.h"
#include "scc/AST/QualType.h"

namespace scc {

enum TagDeclKind { Enum, Struct, Union };

class TagDecl : public TypeDecl {
  public:
    using NameType = TypeDecl::NameType;

  private:
    TagDeclKind TKind;

  public:
    TagDecl(TagDeclKind TKind, NameType Name) : TypeDecl(Tag, Name), TKind(TKind) {}

    TagDeclKind getTagDeclKind() const { return TKind; }
    bool        isEnum() const { return TKind == Enum; }
    bool        isStruct() const { return TKind == Struct; }
    bool        isUnion() const { return TKind == Union; }
    bool        isRecord() const { return isStruct() || isUnion(); }
};

class RecordFieldDecl : public ValueDecl {
  public:
    using NameType = ValueDecl::NameType;

  public:
    RecordFieldDecl(QualType Ty, NameType Name = std::nullopt)
        : ValueDecl(RecordField, Name, Ty) {}
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

class EnumFieldDecl : public NamedDecl {
  public:
    using NameType = NamedDecl::NameType;

  public:
    explicit EnumFieldDecl(NameType Name) : NamedDecl(EnumField, Name) {}
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
