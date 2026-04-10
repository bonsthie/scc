#include "scc/AST/Decl.h"
#include "scc/AST/ArrayType.h"
#include "scc/AST/BuiltinType.h"
#include "scc/AST/PointerType.h"
#include "scc/AST/TagDecl.h"
#include "scc/AST/TagType.h"
#include "scc/AST/TypedefType.h"

#include <cstdint>
#include <ostream>

using namespace scc;

namespace {

constexpr size_t IndentWidth = 2;

const char *decl_kind_name(DeclKind Kind) {
    switch (Kind) {
    case DeclKind::None:
        return "None";
    case DeclKind::Type:
        return "Type";
    case DeclKind::Tag:
        return "Tag";
    case DeclKind::Typedef:
        return "Typedef";
    case DeclKind::Var:
        return "Var";
    case DeclKind::RecordField:
        return "RecordField";
    case DeclKind::EnumField:
        return "EnumField";
    }
    return "<unknown-decl-kind>";
}

const char *tag_decl_kind_name(TagDeclKind Kind) {
    switch (Kind) {
    case Enum:
        return "Enum";
    case Struct:
        return "Struct";
    case Union:
        return "Union";
    }
    return "<unknown-tag-kind>";
}

const char *type_kind_name(TypeKind Kind) {
    switch (Kind) {
    case TypeKind::Uninitialized:
        return "Uninitialized";
    case TypeKind::Builtin:
        return "Builtin";
    case TypeKind::Pointer:
        return "Pointer";
    case TypeKind::Array:
        return "Array";
    case TypeKind::Function:
        return "Function";
    case TypeKind::Enum:
        return "Enum";
    case TypeKind::Record:
        return "Record";
    case TypeKind::Typedef:
        return "Typedef";
    }
    return "<unknown-type-kind>";
}

const char *builtin_type_kind_name(BuiltinTypeKind Kind) {
    switch (Kind) {
    case TYunspecified:
        break;
#define TYPE_KEYWORD(x) \
    case TY##x: \
        return #x;
#include "scc/Token/TokenKinds.def"
    }
    return "<unknown-builtin-type>";
}

void print_name(std::ostream &O, const NamedDecl::NameType &Name) {
    if (Name.has_value()) {
        O << '\'' << *Name << '\'';
        return;
    }
    O << "<anonymous>";
}

void print_indent(std::ostream &O, size_t IndentLevel) {
    for (size_t I = 0; I < IndentLevel * IndentWidth; ++I)
        O << ' ';
}

void print_decl_prefix(std::ostream &O, const char *DeclName, const void *DeclAddress, size_t IndentLevel = 0) {
    print_indent(O, IndentLevel);
    O << DeclName << ' ' << DeclAddress;
}

void print_decl_kind(std::ostream &O, const Decl &Decl) {
    O << " kind=" << decl_kind_name(Decl.kind());
}

void print_named_decl_details(std::ostream &O, const NamedDecl &Decl) {
    print_decl_kind(O, Decl);
    O << " name=";
    print_name(O, Decl.getName());
}

void print_value_decl_details(std::ostream &O, const ValueDecl &Decl);
void print_type(std::ostream &O, const class scc::Type *Ty);
void print_qual_type(std::ostream &O, QualType Ty);
void print_field_decl(std::ostream &O, const RecordFieldDecl &Field, size_t IndentLevel);
void print_enum_field_decl(std::ostream &O, const EnumFieldDecl &Field, size_t IndentLevel);
void print_inline_record_body(std::ostream &O, const RecordDecl &Decl, size_t IndentLevel);
void print_record_fields(std::ostream &O, const RecordDecl &Decl, size_t IndentLevel);
void print_record_decl(std::ostream &O, const char *DeclName, const RecordDecl &Decl, size_t IndentLevel);
void print_enum_fields(std::ostream &O, const EnumDecl &Decl, size_t IndentLevel);
void print_enum_decl(std::ostream &O, const EnumDecl &Decl, size_t IndentLevel);
const void *get_type_address(QualType Ty);

void print_value_decl_details(std::ostream &O, const ValueDecl &Decl) {
    print_named_decl_details(O, Decl);
    O << " type=";
    print_qual_type(O, Decl.getType());
}

const char *tag_decl_keyword(TagDeclKind Kind) {
    switch (Kind) {
    case Enum:
        return "enum";
    case Struct:
        return "struct";
    case Union:
        return "union";
    }
    return "tag";
}

void print_tag_decl_details(std::ostream &O, const TagDecl &Decl) {
    print_decl_kind(O, Decl);
    O << " tag=" << tag_decl_kind_name(Decl.getTagDeclKind()) << " name=";
    print_name(O, Decl.getName());
}

void print_tag_type(std::ostream &O, const TagDecl *Decl) {
    if (!Decl) {
        O << "<null-tag>";
        return;
    }

    O << tag_decl_keyword(Decl->getTagDeclKind());

    if (Decl->getName().has_value())
        O << ' ' << *Decl->getName();
    else
        O << " <anonymous>";
}

void print_array_suffix(std::ostream &O, uint64_t Size) {
    O << '[';
    if (Size != 0)
        O << Size;
    O << ']';
}

const RecordDecl *get_inline_record_decl(QualType Ty) {
    if (Ty.isNull())
        return nullptr;

    while (Ty.kind() == TypeKind::Typedef)
        Ty = Ty.desugarOnce();

    if (Ty.kind() != TypeKind::Record)
        return nullptr;

    auto *RT = static_cast<const RecordType *>(Ty.getType());
    if (!RT || !RT->isInlineRecordType())
        return nullptr;

    const RecordDecl *Decl = RT->getDecl();
    if (!Decl)
        return nullptr;

    return Decl;
}

const char *get_field_decl_name(QualType Ty) {
    if (Ty.isNull())
        return "FieldDecl";

    while (Ty.kind() == TypeKind::Typedef)
        Ty = Ty.desugarOnce();

    if (Ty.kind() != TypeKind::Record)
        return "FieldDecl";

    const auto *RT = static_cast<const RecordType *>(Ty.getType());
    if (RT && RT->isInlineRecordType())
        return "InlineRecordFieldDecl";

    return "RecordFieldDecl";
}

const void *get_type_address(QualType Ty) {
    if (Ty.isNull())
        return nullptr;

    switch (Ty.kind()) {
    case TypeKind::Enum:
    case TypeKind::Record:
        return static_cast<const TagType *>(Ty.getType())->getDecl();
    case TypeKind::Typedef:
        return static_cast<const TypedefType *>(Ty.getType())->getDecl();
    case TypeKind::Builtin:
    case TypeKind::Pointer:
    case TypeKind::Array:
    case TypeKind::Function:
    case TypeKind::Uninitialized:
        return Ty.getType();
    }

    return Ty.getType();
}

void print_type(std::ostream &O, const class scc::Type *Ty) {
    if (!Ty) {
        O << "<null-type>";
        return;
    }

    switch (Ty->kind()) {
    case TypeKind::Builtin:
        O << builtin_type_kind_name(static_cast<const BuiltinType *>(Ty)->getBuiltinKind());
        return;
    case TypeKind::Array: {
        auto *AT = static_cast<const ArrayType *>(Ty);
        print_type(O, AT->getBaseType());
        O << ' ';
        print_array_suffix(O, AT->getSize());
        return;
    }
    case TypeKind::Pointer: {
        auto *PT = static_cast<const PointerType *>(Ty);
        print_type(O, PT->getPointeeType());
        O << " *";
        return;
    }
    case TypeKind::Enum:
        print_tag_type(O, static_cast<const TagType *>(Ty)->getDecl());
        return;
    case TypeKind::Record: {
        auto *RT = static_cast<const RecordType *>(Ty);
        print_tag_type(O, RT->getDecl());
        return;
    }
    case TypeKind::Typedef: {
        auto *TT = static_cast<const TypedefType *>(Ty);
        if (const TypedefDecl *Decl = TT->getDecl(); Decl && Decl->getName().has_value()) {
            O << *Decl->getName();
            return;
        }
        O << "<anonymous-typedef>";
        return;
    }
    case TypeKind::Function:
        O << "<function-type>";
        return;
    case TypeKind::Uninitialized:
        O << type_kind_name(Ty->kind());
        return;
    }

    O << type_kind_name(Ty->kind());
}

void print_qual_type(std::ostream &O, QualType Ty) {
    bool NeedsSpace = false;
    if (Ty.isConstQualified()) {
        O << "const";
        NeedsSpace = true;
    }
    if (Ty.isRestrictQualified()) {
        if (NeedsSpace)
            O << ' ';
        O << "restrict";
        NeedsSpace = true;
    }
    if (Ty.isVolatileQualified()) {
        if (NeedsSpace)
            O << ' ';
        O << "volatile";
        NeedsSpace = true;
    }
    if (NeedsSpace)
        O << ' ';

    print_type(O, Ty.getType());
}

void print_record_decl(std::ostream &O, const char *DeclName, const RecordDecl &Decl, size_t IndentLevel) {
    print_decl_prefix(O, DeclName, &Decl, IndentLevel);
    print_tag_decl_details(O, Decl);
    O << " fields=" << Decl.size();

    if (Decl.empty()) {
        O << '\n';
        return;
    }

    O << " {\n";
    print_record_fields(O, Decl, IndentLevel + 1);
    print_indent(O, IndentLevel);
    O << "}\n";
}

void print_inline_record_body(std::ostream &O, const RecordDecl &Decl, size_t IndentLevel) {
    O << " {\n";
    print_record_fields(O, Decl, IndentLevel + 1);
    print_indent(O, IndentLevel);
    O << "}\n";
}

void print_field_decl(std::ostream &O, const RecordFieldDecl &Field, size_t IndentLevel) {
    const RecordDecl *InlineRecord = get_inline_record_decl(Field.getType());

    print_decl_prefix(O, get_field_decl_name(Field.getType()), &Field, IndentLevel);
    print_value_decl_details(O, Field);

    if (InlineRecord) {
        print_inline_record_body(O, *InlineRecord, IndentLevel);
        return;
    }

    O << '\n';
}

void print_record_fields(std::ostream &O, const RecordDecl &Decl, size_t IndentLevel) {
    for (const RecordFieldDecl &Field : Decl.getFields())
        print_field_decl(O, Field, IndentLevel);
}

void print_enum_field_decl(std::ostream &O, const EnumFieldDecl &Field, size_t IndentLevel) {
    print_decl_prefix(O, "EnumFieldDecl", &Field, IndentLevel);
    print_named_decl_details(O, Field);
    O << '\n';
}

void print_enum_fields(std::ostream &O, const EnumDecl &Decl, size_t IndentLevel) {
    for (const EnumFieldDecl &Field : Decl.getFields())
        print_enum_field_decl(O, Field, IndentLevel);
}

void print_enum_decl(std::ostream &O, const EnumDecl &Decl, size_t IndentLevel) {
    print_decl_prefix(O, "EnumDecl", &Decl, IndentLevel);
    print_tag_decl_details(O, Decl);
    O << " fields=" << Decl.size();

    if (Decl.empty()) {
        O << '\n';
        return;
    }

    O << " {\n";
    print_enum_fields(O, Decl, IndentLevel + 1);
    print_indent(O, IndentLevel);
    O << "}\n";
}

} // namespace

void Decl::print(std::ostream &O) const {
    print_decl_prefix(O, "Decl", this);
    print_decl_kind(O, *this);
    O << '\n';
}

void NamedDecl::print(std::ostream &O) const {
    print_decl_prefix(O, "NamedDecl", this);
    print_named_decl_details(O, *this);
    O << '\n';
}

void TypeDecl::print(std::ostream &O) const {
    print_decl_prefix(O, "TypeDecl", this);
    print_named_decl_details(O, *this);
    O << '\n';
}

void ValueDecl::print(std::ostream &O) const {
    print_decl_prefix(O, "ValueDecl", this);
    print_value_decl_details(O, *this);
    O << '\n';
}

void TypedefDecl::print(std::ostream &O) const {
    QualType Underlying = getUnderlyingType();

    print_decl_prefix(O, "TypedefDecl", this);
    print_named_decl_details(O, *this);
    O << " underlying=";
    print_qual_type(O, Underlying);
    O << " underlying_type=" << get_type_address(Underlying);
    O << '\n';
}

void VarDecl::print(std::ostream &O) const {
    print_decl_prefix(O, "VarDecl", this);
    print_value_decl_details(O, *this);
    O << '\n';
}

void TagDecl::print(std::ostream &O) const {
    print_decl_prefix(O, "TagDecl", this);
    print_tag_decl_details(O, *this);
    O << '\n';
}

void RecordFieldDecl::print(std::ostream &O) const {
    print_field_decl(O, *this, 0);
}

void RecordDecl::print(std::ostream &O) const {
    print_record_decl(O, "RecordDecl", *this, 0);
}

void EnumFieldDecl::print(std::ostream &O) const {
    print_enum_field_decl(O, *this, 0);
}

void EnumDecl::print(std::ostream &O) const {
    print_enum_decl(O, *this, 0);
}
