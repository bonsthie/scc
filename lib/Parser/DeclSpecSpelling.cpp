#include "scc/Parser/DeclSpecSpelling.h"
#include "scc/AST/TagType.h"
#include "scc/AST/TypedefType.h"

using namespace scc;

namespace {

std::string named_type_to_string(const Type *Ty) {
    if (!Ty)
        return {};

    if (Ty->isTypedefType()) {
        const auto *TT = static_cast<const TypedefType *>(Ty);
        if (const auto *Decl = TT->getDecl(); Decl && Decl->getName().has_value())
            return std::string(*Decl->getName());
        return {};
    }

    if (Ty->isEnumType() || Ty->isRecordType()) {
        const auto *TT = static_cast<const TagType *>(Ty);
        if (const auto *Decl = TT->getDecl(); Decl && Decl->getName().has_value())
            return std::string(*Decl->getName());
    }

    return {};
}

} // namespace

std::string scc::storage_class_specifier_to_string(StorageClassSpecifier StorageClass) {
    if (StorageClass == StorageClassSpecifier::Unspecified)
        return "<unspecified-storage-class>";
    return token_spelling(static_cast<tok::TokenKind>(StorageClass));
}

std::string scc::sign_specifier_to_string(SignSpecifier Sign) {
    switch (Sign) {
    case SignSpecifier::Signed:
        return "signed";
    case SignSpecifier::Unsigned:
        return "unsigned";
    case SignSpecifier::Unspecified:
        return "<unspecified-sign>";
    }
    return "<invalid-sign>";
}

std::string scc::length_specifier_to_string(LengthSpecifier Length) {
    switch (Length) {
    case LengthSpecifier::Short:
        return "short";
    case LengthSpecifier::Long:
        return "long";
    case LengthSpecifier::LongLong:
        return "long long";
    case LengthSpecifier::Unspecified:
        return "<unspecified-length>";
    }
    return "<invalid-length>";
}

std::string scc::builtin_type_specifier_to_string(BuiltinTypeKind BuiltinTy) {
    if (BuiltinTy == TYunspecified)
        return "<unspecified-builtin-type>";
    return token_spelling(static_cast<tok::TokenKind>(BuiltinTy));
}

std::string scc::type_to_string(const Type *Ty) {
    if (!Ty)
        return {};

    if (Ty->isBuiltinType())
        return builtin_type_specifier_to_string(
            static_cast<const BuiltinType *>(Ty)->getBuiltinKind());

    if (std::string Name = named_type_to_string(Ty); !Name.empty())
        return Name;

    switch (Ty->kind()) {
    case TypeKind::Uninitialized:
        return "<uninitialized>";
    case TypeKind::Unknow:
        return "<unknow>";
    case TypeKind::Pointer:
        return "<pointer>";
    case TypeKind::Array:
        return "<array>";
    case TypeKind::Function:
        return "<function>";
    case TypeKind::Enum:
        return "<enum>";
    case TypeKind::Record:
        return "<record>";
    case TypeKind::Typedef:
        return "<typedef>";
    case TypeKind::Builtin:
        return "<builtin>";
    }
    return {};
}
