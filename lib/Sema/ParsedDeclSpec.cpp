#include "scc/Sema/ParsedDeclSpec.h"
#include "scc/Frontend/DeclSpecSpelling.h"

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

bool ParsedDeclSpec::hasStorageSpecifier() const {
    return StorageClass != StorageClassSpecifier::Unspecified;
}

bool ParsedDeclSpec::hasSignSpecifier() const {
    return Sign != SignSpecifier::Unspecified;
}

bool ParsedDeclSpec::hasLengthSpecifier() const {
    return Length != LengthSpecifier::Unspecified;
}

bool ParsedDeclSpec::hasTypeSpecifier() const {
    return T != nullptr;
}

StorageClassSpecifier ParsedDeclSpec::getStorageSpecifier() const {
    return StorageClass;
}

tok::TokenKind ParsedDeclSpec::getStorageSpecifierTokenKind() const {
    return static_cast<tok::TokenKind>(StorageClass);
}

const SourceRange &ParsedDeclSpec::getStorageSpecifierRange() const {
    return StorageClassRange;
}

SignSpecifier ParsedDeclSpec::getSignSpecifier() const {
    return Sign;
}

tok::TokenKind ParsedDeclSpec::getSignSpecifierTokenKind() const {
    return static_cast<tok::TokenKind>(Sign);
}

const SourceRange &ParsedDeclSpec::getSignSpecifierRange() const {
    return RangeSign;
}

LengthSpecifier ParsedDeclSpec::getLengthSpecifier() const {
    return Length;
}

tok::TokenKind ParsedDeclSpec::getLengthSpecifierTokenKind() const {
    if (Length == LengthSpecifier::LongLong)
        return tok::t_long;
    return static_cast<tok::TokenKind>(Length);
}

const SourceRange &ParsedDeclSpec::getLengthSpecifierRange() const {
    return LengthRange;
}

const Type *ParsedDeclSpec::getType() const {
    return T;
}

std::string_view ParsedDeclSpec::getTypeName() const {
    return TypeName;
}

const SourceRange &ParsedDeclSpec::getTypeRange() const {
    return TypeSourceRange;
}

const SourceRange &ParsedDeclSpec::getTypeSpecifierRange() const {
    return getTypeRange();
}

void ParsedDeclSpec::setStorageSpecifier(StorageClassSpecifier New) {
    StorageClass = New;
}

void ParsedDeclSpec::setStorageSpecifier(StorageClassSpecifier New, const SourceRange &Range) {
    StorageClass = New;
    StorageClassRange = Range;
}

void ParsedDeclSpec::setSignSpecifier(SignSpecifier New, const SourceRange &Range) {
    Sign = New;
    RangeSign = Range;
}

void ParsedDeclSpec::setLengthSpecifier(LengthSpecifier New) {
    Length = New;
}

void ParsedDeclSpec::setLengthSpecifier(LengthSpecifier New, const SourceRange &Range) {
    Length = New;
    LengthRange = Range;
}

void ParsedDeclSpec::setTypeSpecifier(const Type *NewType, const SourceRange &Range) {
    setTypeSpecifier(NewType, Range, {});
}

void ParsedDeclSpec::setTypeSpecifier(const Type *NewType, const SourceRange &Range,
                                      std::string_view Name) {
    T = NewType;
    TypeName = Name;
    TypeSourceRange = Range;
}

bool ParsedDeclSpec::tryAddStorageSpecifier(StorageClassSpecifier New) {
    if (hasStorageSpecifier())
        return false;
    setStorageSpecifier(New);
    return true;
}

bool ParsedDeclSpec::tryAddStorageSpecifier(StorageClassSpecifier New, const SourceRange &Range) {
    if (hasStorageSpecifier())
        return false;
    setStorageSpecifier(New, Range);
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
