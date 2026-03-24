#ifndef SCC_AST_TYPE_H
#define SCC_AST_TYPE_H

#include <cassert>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>

#include "scc/AST/Qualifiers.h"

namespace scc {

class QualType;

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

class TypedefType : public Type {
    std::string_view Name;
    Type            *UnderlyingTy = nullptr;
    Qualifiers       UnderlyingQuals;

  public:
    TypedefType() : Type(TypeKind::Typedef) {}
    TypedefType(std::string_view Name, Type *UnderlyingTy, Qualifiers Quals = {})
        : Type(TypeKind::Typedef),
          Name(Name),
          UnderlyingTy(UnderlyingTy),
          UnderlyingQuals(Quals) {
        assert(!Name.empty() && "scc::TypedefType Name can't be empty");
        assert(UnderlyingTy && "scc::TypedefType Type can't be null");
    }

    std::string_view getName() const { return Name; }

    QualType getUnderlyingType() const;
};

class ArrayType : public Type {
    Type    *BaseTy = nullptr;
    uint64_t Size = 0;

  public:
    ArrayType() : Type(TypeKind::Array) {}
    ArrayType(Type *BaseTy, uint64_t Size) : Type(TypeKind::Array), BaseTy(BaseTy), Size(Size) {}

    void     setBaseType(Type *T) { BaseTy = T; }
    Type    *getBaseType() const { return BaseTy; }
    void     setSize(uint64_t N) { Size = N; }
    uint64_t getSize() const { return Size; }
};

class RecordType : public Type {
    uint64_t ElemSize = 0;
    void    *ElementsData = nullptr;

  public:
    using Element = std::optional<std::string_view>;
    using Elements = std::span<Element>;
};

} // namespace scc

#endif // SCC_AST_TYPE_H
