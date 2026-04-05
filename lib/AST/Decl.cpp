#include "scc/AST/Decl.h"
#include "scc/AST/ArrayType.h"
#include "scc/AST/BuiltinType.h"
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
    case None:
        return "None";
    case Type:
        return "Type";
    case Tag:
        return "Tag";
    case Typedef:
        return "Typedef";
    case Var:
        return "Var";
    case RecordField:
        return "RecordField";
    case EnumField:
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

void print_type(std::ostream &O, const class scc::Type *Ty);
void print_record_field(std::ostream &O, const RecordFieldDecl &Field, size_t IndentLevel);
void print_record_fields(std::ostream &O, const RecordDecl &Decl, size_t IndentLevel);
void print_record_decl(std::ostream &O, const char *DeclName, const RecordDecl &Decl, size_t IndentLevel);

void print_tag_type(std::ostream &O, const TagDecl *Decl) {
    if (!Decl) {
        O << "<null-tag>";
        return;
    }

    if (Decl->isStruct())
        O << "struct";
    else if (Decl->isUnion())
        O << "union";
    else if (Decl->isEnum())
        O << "enum";
    else
        O << "tag";

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
    if (Ty.isNull() || Ty.kind() != TypeKind::Record)
        return nullptr;

    auto *RT = static_cast<const RecordType *>(Ty.getType());
    if (!RT || !RT->isInlineRecordType())
        return nullptr;

    const RecordDecl *Decl = RT->getDecl();
    if (!Decl)
        return nullptr;

    return Decl;
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
    case TypeKind::Pointer:
        O << "<pointer-type>";
        return;
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
    print_indent(O, IndentLevel);
    O << DeclName << ' ' << &Decl << " tag=" << tag_decl_kind_name(Decl.getTagDeclKind()) << " name=";
    print_name(O, Decl.getName());
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

void print_record_field(std::ostream &O, const RecordFieldDecl &Field, size_t IndentLevel) {
    print_indent(O, IndentLevel);
    O << "RecordFieldDecl " << &Field << " name=";
    print_name(O, Field.getName());
    O << " type=";
    print_qual_type(O, Field.getType());

    if (const RecordDecl *InlineRecord = get_inline_record_decl(Field.getType())) {
        O << '\n';
        print_record_decl(O, "InlineRecordDecl", *InlineRecord, IndentLevel);
        return;
    }

    O << '\n';
}

void print_record_fields(std::ostream &O, const RecordDecl &Decl, size_t IndentLevel) {
    for (const RecordFieldDecl &Field : Decl.getFields())
        print_record_field(O, Field, IndentLevel);
}

} // namespace

void Decl::print(std::ostream &O) const {
    O << "Decl " << this << " kind=" << decl_kind_name(kind()) << '\n';
}

void NamedDecl::print(std::ostream &O) const {
    O << "NamedDecl " << this << " kind=" << decl_kind_name(kind()) << " name=";
    print_name(O, getName());
    O << '\n';
}

void TypeDecl::print(std::ostream &O) const {
    O << "TypeDecl " << this << " kind=" << decl_kind_name(kind()) << " name=";
    print_name(O, getName());
    O << '\n';
}

void ValueDecl::print(std::ostream &O) const {
    O << "ValueDecl " << this << " kind=" << decl_kind_name(kind()) << " name=";
    print_name(O, getName());
    O << " type=";
    print_qual_type(O, getType());
    O << '\n';
}

void TypedefDecl::print(std::ostream &O) const {
    O << "TypedefDecl " << this << " name=";
    print_name(O, getName());
    O << " underlying=";
    print_qual_type(O, getUnderlyingType());

    if (const RecordDecl *InlineRecord = get_inline_record_decl(getUnderlyingType())) {
        O << '\n';
        print_record_decl(O, "InlineRecordDecl", *InlineRecord, 0);
        return;
    }

    O << '\n';
}

void VarDecl::print(std::ostream &O) const {
    O << "VarDecl " << this << " name=";
    print_name(O, getName());
    O << " type=";
    print_qual_type(O, getType());
    O << '\n';
}

void TagDecl::print(std::ostream &O) const {
    O << "TagDecl " << this << " tag=" << tag_decl_kind_name(getTagDeclKind()) << " name=";
    print_name(O, getName());
    O << '\n';
}

void RecordFieldDecl::print(std::ostream &O) const {
    print_record_field(O, *this, 0);
}

void RecordDecl::print(std::ostream &O) const {
    print_record_decl(O, "RecordDecl", *this, 0);
}

void EnumFieldDecl::print(std::ostream &O) const {
    O << "EnumFieldDecl " << this << " name=";
    print_name(O, getName());
    O << '\n';
}

void EnumDecl::print(std::ostream &O) const {
    O << "EnumDecl " << this << " name=";
    print_name(O, getName());
    O << " fields=" << size();

    if (empty()) {
        O << '\n';
        return;
    }

    O << " {\n";
    for (const EnumFieldDecl &Field : getFields()) {
        O << "  ";
        Field.print(O);
    }
    O << "}\n";
}
