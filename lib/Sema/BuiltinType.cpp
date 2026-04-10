#include "scc/AST/BuiltinType.h"
#include "scc/ADT/TableBuilder.h"
#include "scc/AST/CanQualType.h"
#include "scc/Sema/Sema.h"

#include <cassert>

using namespace scc;

namespace {

#define TYPE_KEYWORD(x)                                                                            \
    static constinit BuiltinType __BuliltinType_##x(TY##x);                                        \
    static constexpr CanQualType Ty##x{QualType(&__BuliltinType_##x)};
#include "scc/Token/TokenKinds.def"

static constexpr auto BuiltinTypeTable = //
    TableBuilder<tok::TokenKind, const CanQualType *, static_cast<size_t>(tok::NUMBER_OF_TOKENS)> {}
#define TYPE_KEYWORD(x) .add(tok::t_##x, &Ty##x)
#include "scc/Token/TokenKinds.def"
.build();

} // namespace

const CanQualType *Sema::getBuiltinType(BuiltinTypeKind BType) {
    const CanQualType *QT = BuiltinTypeTable[static_cast<size_t>(BType)];
    assert(QT && "Sema::getBuiltinType requires a valid builtin type kind");
    return QT;
}
