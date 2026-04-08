#ifndef SCC_PARSER_PARSEDDECLSPEC_H
#define SCC_PARSER_PARSEDDECLSPEC_H

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
};

struct ParsedDeclSpec {
    StorageClassSpecifier StorageClass = StorageClassSpecifier::Unspecified;

    SignSpecifier Sign = SignSpecifier::Unspecified;
    SourceRange   RangeSign; // use by sema to emit error if the type can't have a Sign Specifier

    LengthSpecifier Length = LengthSpecifier::Unspecified;
    SourceRange     LengthRange;

    Type      *T;
    Qualifiers Quals;

    bool hasStorageSpecifier() const { return StorageClass != StorageClassSpecifier::Unspecified; }
    bool hasSignSpecifier() const { return Sign != SignSpecifier::Unspecified; }
    bool hasLengthSpecifier() const { return Length != LengthSpecifier::Unspecified; }
    bool hasTypeSpecifier() const { return T != nullptr; }

    void setStorageSpecifier(StorageClassSpecifier New) { StorageClass = New; }
    void setSignSpecifier(SignSpecifier New, const SourceRange &Range = {}) {
        Sign = New;
        RangeSign = Range;
    }
    void setLengthSpecifier(LengthSpecifier New, const SourceRange &Range = {}) {
        Length = New;
        LengthRange = Range;
    }
    void setTypeSpecifier(Type *NewType) { T = NewType; }

    bool tryAddStorageSpecifier(StorageClassSpecifier New);
    bool trySetStorageSpecifier(StorageClassSpecifier New);
    bool tryAddSignSpecifier(SignSpecifier New, const SourceRange &Range = {});
    bool tryAddLengthSpecifier(LengthSpecifier New, const SourceRange &Range = {});
    bool tryAddTypeSpecifier(Type *New);
    bool tryAddConst();
    bool tryAddRestrict();
    bool tryAddVolatile();
    bool tryAddQualifiers(const Qualifiers &Other);
};

} // namespace scc

#endif // SCC_PARSER_PARSEDDECLSPEC_H
