#include "scc/AST/BuiltinType.h"
#include "scc/Frontend/DeclSpecSpelling.h"
#include "scc/Sema/Sema.h"

#include <string>

using namespace scc;

namespace {

bool hasSignSpecifier(const ParsedDeclSpec &DS) { return DS.Sign != SignSpecifier::Unspecified; }

bool hasLengthSpecifier(const ParsedDeclSpec &DS) {
    return DS.Length != LengthSpecifier::Unspecified;
}

SourceRange getImplicitIntRange(const ParsedDeclSpec &DS) {
    if (DS.TypeSourceRange.isValid())
        return DS.TypeSourceRange;
    if (hasSignSpecifier(DS))
        return DS.RangeSign;
    if (hasLengthSpecifier(DS))
        return DS.LengthRange;
    return {};
}

} // namespace

bool Sema::actOnDeclSpecType(ParsedDeclSpec &DS) {
    const Type *T = DS.T;
    if (T != nullptr && T->kind() != TypeKind::Unknow)
        return false;

    if (T != nullptr && T->kind() == TypeKind::Unknow) {
        EM.unknownTypeName(DS.TypeName, DS.TypeSourceRange);
        return true;
    }

    SourceRange Range = getImplicitIntRange(DS);
    if (hasLengthSpecifier(DS) || hasSignSpecifier(DS) || Opts.implicit_intEnable) {
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
    if (!hasLengthSpecifier(DS) && !hasSignSpecifier(DS))
        return false;

    const Type *T = DS.T;
    if (!T || !T->isBuiltinType())
        return false;

    auto        Kind = static_cast<const BuiltinType *>(T)->getBuiltinKind();
    std::string TypeName = builtin_type_specifier_to_string(Kind);

    switch (Kind) {
    case TYchar:
        if (hasLengthSpecifier(DS)) {
            EM.invalidTypeModifier(length_specifier_to_string(DS.Length), TypeName, DS.LengthRange);
            return true;
        }
        return false;

    case TYint:
        return false;

    case TYdouble:
        if (hasSignSpecifier(DS)) {
            EM.invalidTypeModifier(sign_specifier_to_string(DS.Sign), TypeName, DS.RangeSign);
            return true;
        }
        if (DS.Length == LengthSpecifier::Short || DS.Length == LengthSpecifier::LongLong) {
            EM.invalidTypeModifier(length_specifier_to_string(DS.Length), TypeName, DS.LengthRange);
            return true;
        }
        return false;

    default:
        if (hasSignSpecifier(DS)) {
            EM.invalidTypeModifier(sign_specifier_to_string(DS.Sign), TypeName, DS.RangeSign);
            return true;
        }
        EM.invalidTypeModifier(length_specifier_to_string(DS.Length), TypeName, DS.LengthRange);
        return true;
    }
}

bool Sema::actOnDeclSpec(ParsedDeclSpec &DS) {
    bool err = false;
    // TODO verify at the start the _Imaginary and Complex that can only be float or double

    err |= actOnDeclSpecType(DS);
    err |= actOnDeclSpecLengthAndSignSpecifier(DS);

    return err;
}
