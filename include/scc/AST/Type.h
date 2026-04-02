#ifndef SCC_AST_TYPE_H
#define SCC_AST_TYPE_H

namespace scc {

enum class TypeKind {
    Uninitialized,
    Builtin,  // int, char, float, void
    Pointer,  // int*
    Array,    // int[10]
    Function, // int(int, float)
    Record,   // struct { ... } union { ... }
    Typedef,  // typedef
};

class Type {
    TypeKind Kind;

  public:
    explicit Type(TypeKind Kind) : Kind(Kind) {}

    TypeKind kind() const { return Kind; }

    bool isBuiltinType() const { return Kind == TypeKind::Builtin; }
    bool isPointerType() const { return Kind == TypeKind::Pointer; }
    bool isArrayType() const { return Kind == TypeKind::Array; }
    bool isRecordType() const { return Kind == TypeKind::Record; }
    bool isTypedefType() const { return Kind == TypeKind::Typedef; }
    bool isVoidType() const { return false; }; // TODO by checking if builtin type
};

} // namespace scc

#endif // SCC_AST_TYPE_H
