#include "scc/Parser/ParsedDeclSpec.h"
#include "scc/Parser/DeclSpecSpelling.h"

using namespace scc;

namespace {

void print_word(std::ostream &O, bool &NeedsSpace, std::string_view Word) {
    if (Word.empty())
        return;
    if (NeedsSpace)
        O << ' ';
    O << Word;
    NeedsSpace = true;
}

} // namespace

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
    if (!hasLengthSpecifier()) {
        setLengthSpecifier(New, Range);
        return true;
    }

    if (Length == LengthSpecifier::Long && New == LengthSpecifier::Long) {
        setLengthSpecifier(LengthSpecifier::LongLong);
        return true;
    }

    return false;
}

bool ParsedDeclSpec::tryAddTypeSpecifier(const Type *New, const SourceRange &Range) {
    if (New == nullptr || T != nullptr)
        return false;
    setTypeSpecifier(New, Range);
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

void ParsedDeclSpec::print(std::ostream &O) const {
    bool NeedsSpace = false;

    if (hasStorageSpecifier())
        print_word(O, NeedsSpace, storage_class_specifier_to_string(StorageClass));
    if (hasSignSpecifier())
        print_word(O, NeedsSpace, sign_specifier_to_string(Sign));
    if (hasLengthSpecifier())
        print_word(O, NeedsSpace, length_specifier_to_string(Length));
    if (Quals.IsConst)
        print_word(O, NeedsSpace, "const");
    if (Quals.IsRestrict)
        print_word(O, NeedsSpace, "restrict");
    if (Quals.IsVolatile)
        print_word(O, NeedsSpace, "volatile");
    if (T)
        print_word(O, NeedsSpace, type_to_string(T));
}
