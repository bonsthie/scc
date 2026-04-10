#ifndef SCC_AST_TYPE_H
#define SCC_AST_TYPE_H

namespace scc {

enum class TypeKind {
    Unknow = -1,
    Uninitialized,
    Builtin,  // int, char, float, void
    Pointer,  // int*
    Array,    // int[10]
    Function, // int(int, float)
    Enum,     // enum E
    Record,   // struct { ... } union { ... }
    Typedef,  // typedef
};

class Type {
    TypeKind Kind;

  public:
    explicit constexpr Type(TypeKind Kind) : Kind(Kind) {}

    constexpr TypeKind kind() const { return Kind; }

    constexpr bool isBuiltinType() const { return Kind == TypeKind::Builtin; }
    constexpr bool isPointerType() const { return Kind == TypeKind::Pointer; }
    constexpr bool isArrayType() const { return Kind == TypeKind::Array; }
    constexpr bool isEnumType() const { return Kind == TypeKind::Enum; }
    constexpr bool isRecordType() const { return Kind == TypeKind::Record; }
    constexpr bool isTypedefType() const { return Kind == TypeKind::Typedef; }
    constexpr bool isUnknow() const { return Kind == TypeKind::Unknow; }
    constexpr bool isVoidType() const { return false; }; // TODO by checking if builtin type
};

} // namespace scc

#endif // SCC_AST_TYPE_H
