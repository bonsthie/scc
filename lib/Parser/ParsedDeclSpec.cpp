#include "scc/Parser/ParsedDeclSpec.h"

using namespace scc;

bool ParsedDeclSpec::tryAddStorageSpecifier(StorageClassSpecifier New) {
    if (hasStorageSpecifier())
        return false;
    setStorageSpecifier(New);
    return true;
}

bool ParsedDeclSpec::trySetStorageSpecifier(StorageClassSpecifier New) {
    return tryAddStorageSpecifier(New);
}

bool ParsedDeclSpec::tryAddSignSpecifier(SignSpecifier New, const SourceRange &Range) {
    if (hasSignSpecifier())
        return false;
    setSignSpecifier(New, Range);
    return true;
}

bool ParsedDeclSpec::tryAddLengthSpecifier(LengthSpecifier New, const SourceRange &Range) {
    if (hasLengthSpecifier())
        return false;
    setLengthSpecifier(New, Range);
    return true;
}

bool ParsedDeclSpec::tryAddTypeSpecifier(Type *New) {
    if (hasTypeSpecifier())
        return false;
    setTypeSpecifier(New);
    return true;
}

bool ParsedDeclSpec::tryAddConst() {
    if (Quals.IsConst)
        return false;
    Quals.IsConst = true;
    return true;
}

bool ParsedDeclSpec::tryAddRestrict() {
    if (Quals.IsRestrict)
        return false;
    Quals.IsRestrict = true;
    return true;
}

bool ParsedDeclSpec::tryAddVolatile() {
    if (Quals.IsVolatile)
        return false;
    Quals.IsVolatile = true;
    return true;
}

bool ParsedDeclSpec::tryAddQualifiers(const Qualifiers &Other) {
    if ((Quals.IsConst && Other.IsConst) || (Quals.IsRestrict && Other.IsRestrict) ||
        (Quals.IsVolatile && Other.IsVolatile))
        return false;
    Quals.merge(Other);
    return true;
}
