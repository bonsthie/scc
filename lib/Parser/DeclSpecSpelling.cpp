#include "scc/Parser/DeclSpecSpelling.h"

using namespace scc;

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
        return builtin_type_specifier_to_string(static_cast<const BuiltinType *>(Ty)->getBuiltinKind());

    switch (Ty->kind()) {
    case TypeKind::Uninitialized:
        return "uninitialized";
    case TypeKind::Pointer:
        return "pointer";
    case TypeKind::Array:
        return "array";
    case TypeKind::Function:
        return "function";
    case TypeKind::Enum:
        return "enum";
    case TypeKind::Record:
        return "record";
    case TypeKind::Typedef:
        return "typedef";
    case TypeKind::Builtin:
        return "builtin";
    }
    return {};
}
