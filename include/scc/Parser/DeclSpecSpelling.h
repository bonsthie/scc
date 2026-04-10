#ifndef SCC_PARSER_DECLSPECSPELLING_H
#define SCC_PARSER_DECLSPECSPELLING_H

#include <string>

#include "scc/AST/BuiltinType.h"
#include "scc/AST/Type.h"
#include "scc/Parser/ParsedDeclSpec.h"

namespace scc {

std::string storage_class_specifier_to_string(StorageClassSpecifier StorageClass);
std::string sign_specifier_to_string(SignSpecifier Sign);
std::string length_specifier_to_string(LengthSpecifier Length);
std::string builtin_type_specifier_to_string(BuiltinTypeKind BuiltinTy);
std::string type_to_string(const Type *Ty);

} // namespace scc

#endif // SCC_PARSER_DECLSPECSPELLING_H
