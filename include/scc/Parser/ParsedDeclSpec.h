#ifndef SCC_PARSER_PARSEDDECLSPEC_H
#define SCC_PARSER_PARSEDDECLSPEC_H

#include <iostream>

#include "scc/AST/BuiltinType.h"
#include "scc/AST/Qualifiers.h"
#include "scc/AST/Type.h"
#include "scc/Token/Token.h"

namespace scc {

enum class StorageClassSpecifier : int {
    Unspecified = tok::not_init,
    Auto = tok::kw_auto,
    Typedef = tok::kw_typedef,
    Static = tok::kw_static,
    Extern = tok::kw_extern,
    Register = tok::kw_register,
};

enum class SignSpecifier : int {
    Unspecified = tok::not_init,
    Signed = tok::t_signed,
    Unsigned = tok::t_unsigned,
};

enum class LengthSpecifier : int {
    Unspecified = tok::not_init,
    Short = tok::t_short,
    Long = tok::t_long,
    LongLong = tok::t_long + tok::t_long
};

struct ParsedDeclSpec {
    StorageClassSpecifier StorageClass = StorageClassSpecifier::Unspecified;

    SignSpecifier Sign = SignSpecifier::Unspecified;
    SourceRange   RangeSign; // use by sema to emit error if the type can't have a Sign Specifier

    LengthSpecifier Length = LengthSpecifier::Unspecified;
    SourceRange     LengthRange;

    const Type       *T = nullptr;
    Qualifiers  Quals;
    SourceRange TypeSourceRange;

    bool hasStorageSpecifier() const { return StorageClass != StorageClassSpecifier::Unspecified; }
    bool hasSignSpecifier() const { return Sign != SignSpecifier::Unspecified; }
    bool hasLengthSpecifier() const { return Length != LengthSpecifier::Unspecified; }
    bool hasTypeSpecifier() const { return T != nullptr; }
    StorageClassSpecifier getStorageSpecifier() const { return StorageClass; }
    tok::TokenKind        getStorageSpecifierTokenKind() const {
        return static_cast<tok::TokenKind>(StorageClass);
    }
    SignSpecifier  getSignSpecifier() const { return Sign; }
    tok::TokenKind getSignSpecifierTokenKind() const { return static_cast<tok::TokenKind>(Sign); }
    const SourceRange &getSignSpecifierRange() const { return RangeSign; }
    LengthSpecifier    getLengthSpecifier() const { return Length; }
    tok::TokenKind     getLengthSpecifierTokenKind() const {
        if (Length == LengthSpecifier::LongLong)
            return tok::t_long;
        return static_cast<tok::TokenKind>(Length);
    }
    const SourceRange &getLengthSpecifierRange() const { return LengthRange; }

    void setStorageSpecifier(StorageClassSpecifier New) { StorageClass = New; }
    void setSignSpecifier(SignSpecifier New, const SourceRange &Range) {
        Sign = New;
        RangeSign = Range;
    }

    void setLengthSpecifier(LengthSpecifier New) { Length = New; }
    void setLengthSpecifier(LengthSpecifier New, const SourceRange &Range) {
        Length = New;
        LengthRange = Range;
    }

    void setTypeSpecifier(const Type *NewType, const SourceRange &Range) {
        T = NewType;
        TypeSourceRange = Range;
    }

    bool tryAddStorageSpecifier(StorageClassSpecifier New);
    bool trySetStorageSpecifier(StorageClassSpecifier New);
    bool tryAddSignSpecifier(SignSpecifier New, const SourceRange &Range);
    bool tryAddLengthSpecifier(LengthSpecifier New, const SourceRange &Range);
    bool tryAddTypeSpecifier(const Type *New, const SourceRange &Range);
    bool tryAddConst();
    bool tryAddRestrict();
    bool tryAddVolatile();
    bool tryAddQualifiers(const Qualifiers &Other);
    void print(std::ostream &O = std::cout) const;
};

} // namespace scc

#endif // SCC_PARSER_PARSEDDECLSPEC_H
