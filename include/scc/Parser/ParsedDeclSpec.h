#ifndef SCC_PARSER_PARSEDDECLSPEC_H
#define SCC_PARSER_PARSEDDECLSPEC_H

#include "scc/AST/Qualifiers.h"

namespace scc {

enum class StorageClassSpecifier {
    Unspecified,
    Auto,
    Typedef,
    Static,
    Extern,
    Register,
};

enum class SignSpecifier {
    Unspecified,
    Signed,
    Unsigned,
};

enum class BuiltinTypeSpecifier {
    Unspecified,
    Void,
    Bool,
    Char,
    Short,
    Int,
    Long,
    Float,
    Double,
    Complex,
    Imaginary,
};

struct ParsedDeclSpec {
    StorageClassSpecifier StorageClass = StorageClassSpecifier::Unspecified;
    SignSpecifier         Sign = SignSpecifier::Unspecified;
    BuiltinTypeSpecifier  BuiltinType = BuiltinTypeSpecifier::Unspecified;
    Qualifiers            Quals;

    bool hasStorageClass() const { return StorageClass != StorageClassSpecifier::Unspecified; }
    bool hasSignSpecifier() const { return Sign != SignSpecifier::Unspecified; }
    bool hasBuiltinTypeSpecifier() const {
        return BuiltinType != BuiltinTypeSpecifier::Unspecified;
    }

    void setStorageClass(StorageClassSpecifier Spec) { StorageClass = Spec; }
    void setSignSpecifier(SignSpecifier Spec) { Sign = Spec; }
    void setBuiltinTypeSpecifier(BuiltinTypeSpecifier Spec) { BuiltinType = Spec; }

    bool trySetStorageSpecifier(StorageClassSpecifier Spec) {
        if (hasStorageClass())
            return false;
        setStorageClass(Spec);
        return true;
    }

    bool trySetSignSpecifier(SignSpecifier Spec) {
        if (hasSignSpecifier())
            return false;
        setSignSpecifier(Spec);
        return true;
    }

    bool trySetBuiltinTypeSpecifier(BuiltinTypeSpecifier Spec) {
        if (hasBuiltinTypeSpecifier())
            return false;
        setBuiltinTypeSpecifier(Spec);
        return true;
    }

    void addConst() { Quals.IsConst = true; }
    void addRestrict() { Quals.IsRestrict = true; }
    void addVolatile() { Quals.IsVolatile = true; }
    void addQualifiers(const Qualifiers &Other) { Quals.merge(Other); }
    void setConstQualified(bool Value = true) { Quals.IsConst = Value; }
    void setRestrictQualified(bool Value = true) { Quals.IsRestrict = Value; }
    void setVolatileQualified(bool Value = true) { Quals.IsVolatile = Value; }

    bool trySetConstQualified() {
        if (Quals.IsConst)
            return false;
        addConst();
        return true;
    }

    bool trySetRestrictQualified() {
        if (Quals.IsRestrict)
            return false;
        addRestrict();
        return true;
    }

    bool trySetVolatileQualified() {
        if (Quals.IsVolatile)
            return false;
        addVolatile();
        return true;
    }

    bool tryAddQualifiers(const Qualifiers &Other) {
        if ((Quals.IsConst && Other.IsConst) || (Quals.IsRestrict && Other.IsRestrict) ||
            (Quals.IsVolatile && Other.IsVolatile))
            return false;
        addQualifiers(Other);
        return true;
    }
};

} // namespace scc

#endif // SCC_PARSER_PARSEDDECLSPEC_H
