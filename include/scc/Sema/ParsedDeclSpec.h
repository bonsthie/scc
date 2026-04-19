#ifndef SCC_SEMA_PARSEDDECLSPEC_H
#define SCC_SEMA_PARSEDDECLSPEC_H

#include <iostream>
#include <string_view>

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
    SourceRange           StorageClassRange;

    SignSpecifier Sign = SignSpecifier::Unspecified;
    SourceRange   RangeSign; // use by sema to emit error if the type can't have a Sign Specifier

    LengthSpecifier Length = LengthSpecifier::Unspecified;
    SourceRange     LengthRange;

    const Type *T = nullptr;
    std::string_view TypeName;
    Qualifiers  Quals;
    SourceRange TypeSourceRange;

    bool hasStorageSpecifier() const;
    bool hasSignSpecifier() const;
    bool hasLengthSpecifier() const;
    bool hasTypeSpecifier() const;

    StorageClassSpecifier getStorageSpecifier() const;
    tok::TokenKind        getStorageSpecifierTokenKind() const;
    const SourceRange    &getStorageSpecifierRange() const;
    SignSpecifier         getSignSpecifier() const;
    tok::TokenKind        getSignSpecifierTokenKind() const;
    const SourceRange    &getSignSpecifierRange() const;
    LengthSpecifier       getLengthSpecifier() const;
    tok::TokenKind        getLengthSpecifierTokenKind() const;
    const SourceRange    &getLengthSpecifierRange() const;
    const Type           *getType() const;
    std::string_view      getTypeName() const;
    const SourceRange    &getTypeRange() const;
    const SourceRange    &getTypeSpecifierRange() const;

    void setStorageSpecifier(StorageClassSpecifier New);
    void setStorageSpecifier(StorageClassSpecifier New, const SourceRange &Range);
    void setSignSpecifier(SignSpecifier New, const SourceRange &Range);

    void setLengthSpecifier(LengthSpecifier New);
    void setLengthSpecifier(LengthSpecifier New, const SourceRange &Range);

    void setTypeSpecifier(const Type *NewType, const SourceRange &Range);
    void setTypeSpecifier(const Type *NewType, const SourceRange &Range, std::string_view Name);

    bool tryAddStorageSpecifier(StorageClassSpecifier New);
    bool tryAddStorageSpecifier(StorageClassSpecifier New, const SourceRange &Range);
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

#endif // SCC_SEMA_PARSEDDECLSPEC_H
