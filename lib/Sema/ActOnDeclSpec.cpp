#include "scc/AST/BuiltinType.h"
#include "scc/Frontend/DeclSpecSpelling.h"
#include "scc/Sema/Sema.h"

#include <string>

using namespace scc;

static bool has_sign_specifier(const ParsedDeclSpec &DS) {
    return DS.Sign != SignSpecifier::Unspecified;
}

static bool has_length_specifier(const ParsedDeclSpec &DS) {
    return DS.Length != LengthSpecifier::Unspecified;
}

static SourceRange get_implicit_int_range(const ParsedDeclSpec &DS) {
    if (DS.TypeSourceRange.isValid())
        return DS.TypeSourceRange;
    if (has_sign_specifier(DS))
        return DS.RangeSign;
    if (has_length_specifier(DS))
        return DS.LengthRange;
    return {};
}

bool Sema::actOnDeclSpecType(ParsedDeclSpec &DS) {
    const Type *T = DS.T;
    if (T != nullptr && T->kind() != TypeKind::Unknow)
        return false;

    if (T != nullptr && T->kind() == TypeKind::Unknow) {
        EM.unknownTypeName(DS.TypeName, DS.TypeSourceRange);
        return true;
    }

    SourceRange Range = get_implicit_int_range(DS);
    if (has_length_specifier(DS) || has_sign_specifier(DS) || Opts.implicit_intEnable) {
        DS.T = getType(BuiltinTypeKind::TYint);
        DS.TypeSourceRange = Range;
        return false;
    }

    if (Opts.implicit_intWarning) {
        EM.missingTypeSpecifier(Range);
        DS.T = getType(BuiltinTypeKind::TYint);
        DS.TypeSourceRange = Range;
        return true;
    }

    DS.T = getType(BuiltinTypeKind::TYint);
    DS.TypeSourceRange = Range;
    return false;
}

bool Sema::actOnDeclSpecLengthAndSignSpecifier(ParsedDeclSpec &DS) {
    if (!has_length_specifier(DS) && !has_sign_specifier(DS))
        return false;

    const Type *T = DS.T;
    if (!T || !T->isBuiltinType())
        return false;

    auto        Kind = static_cast<const BuiltinType *>(T)->getBuiltinKind();
    std::string TypeName = builtin_type_specifier_to_string(Kind);

    switch (Kind) {
    case TYchar:
        if (has_length_specifier(DS)) {
            EM.invalidTypeModifier(length_specifier_to_string(DS.Length), TypeName, DS.LengthRange);
            return true;
        }
        return false;

    case TYint:
        return false;

    case TYdouble:
        if (has_sign_specifier(DS)) {
            EM.invalidTypeModifier(sign_specifier_to_string(DS.Sign), TypeName, DS.RangeSign);
            return true;
        }
        if (DS.Length == LengthSpecifier::Short || DS.Length == LengthSpecifier::LongLong) {
            EM.invalidTypeModifier(length_specifier_to_string(DS.Length), TypeName, DS.LengthRange);
            return true;
        }
        return false;

    default:
        if (has_sign_specifier(DS)) {
            EM.invalidTypeModifier(sign_specifier_to_string(DS.Sign), TypeName, DS.RangeSign);
            return true;
        }
        EM.invalidTypeModifier(length_specifier_to_string(DS.Length), TypeName, DS.LengthRange);
        return true;
    }
}

bool Sema::actOnDeclSpec(ParsedDeclSpec &DS) {
    bool Err = false;
    // TODO verify at the start the _Imaginary and Complex that can only be float or double

    Err |= actOnDeclSpecType(DS);
    Err |= actOnDeclSpecLengthAndSignSpecifier(DS);

    return Err;
}
