#ifndef SCC_AST_RECORDTYPE_H
#define SCC_AST_RECORDTYPE_H

#include <optional>
#include <string_view>

#include "scc/ADT/Span.h"
#include "scc/AST/QualType.h"

namespace scc {

enum RecordTypeKind { RTy_struct, RTy_union };

class RecordType : public Type {
  public:
    struct Element {
        QualType                        Ty;
        std::optional<std::string_view> Name;
    };

    using Elements = Span<Element>;
    using ConstElements = Span<const Element>;

  private:
    RecordTypeKind                  RKind = RTy_struct;
    std::optional<std::string_view> Name;
    Element                        *ElementsData = nullptr;
    size_t                          ElementCount = 0;

  protected:
    explicit RecordType(RecordTypeKind RKind) : Type(TypeKind::Record), RKind(RKind) {}

    RecordType(RecordTypeKind RKind, std::optional<std::string_view> Name, Element *ElementsData,
               size_t ElementCount)
        : Type(TypeKind::Record),
          RKind(RKind),
          Name(Name),
          ElementsData(ElementsData),
          ElementCount(ElementCount) {}

    RecordType(RecordTypeKind RKind, Element *ElementsData, size_t ElementCount)
        : RecordType(RKind, std::nullopt, ElementsData, ElementCount) {}

    explicit RecordType(RecordTypeKind RKind, Elements Elements,
                        std::optional<std::string_view> Name = std::nullopt)
        : RecordType(RKind, Name, Elements.data(), Elements.size()) {}

  public:
    const std::optional<std::string_view> &getName() const { return Name; }
    void                                   setName(std::optional<std::string_view> NewName) {
        Name = NewName;
    }
    bool isAnonymous() const { return !Name.has_value(); }

    Elements      getElements() { return {ElementsData, ElementCount}; }
    ConstElements getElements() const { return {ElementsData, ElementCount}; }

    RecordTypeKind getRecordTypeKind() const { return RKind; }
    bool           isStruct() const { return RKind == RTy_struct; }
    bool           isUnion() const { return RKind == RTy_union; }

    void setElements(Element *Data, size_t Count) {
        ElementsData = Data;
        ElementCount = Count;
    }

    void setElements(Elements Elements) {
        ElementsData = Elements.data();
        ElementCount = Elements.size();
    }

    bool   empty() const { return ElementCount == 0; }
    size_t size() const { return ElementCount; }
};

struct RecordStruct : RecordType {
    RecordStruct() : RecordType(RTy_struct) {}

    RecordStruct(std::optional<std::string_view> Name, Element *ElementsData, size_t ElementCount)
        : RecordType(RTy_struct, Name, ElementsData, ElementCount) {}

    RecordStruct(Element *ElementsData, size_t ElementCount)
        : RecordType(RTy_struct, ElementsData, ElementCount) {}

    explicit RecordStruct(Elements Elements, std::optional<std::string_view> Name = std::nullopt)
        : RecordType(RTy_struct, Elements, Name) {}
};

struct RecordUnion : RecordType {
    RecordUnion() : RecordType(RTy_union) {}

    RecordUnion(std::optional<std::string_view> Name, Element *ElementsData, size_t ElementCount)
        : RecordType(RTy_union, Name, ElementsData, ElementCount) {}

    RecordUnion(Element *ElementsData, size_t ElementCount)
        : RecordType(RTy_union, ElementsData, ElementCount) {}

    explicit RecordUnion(Elements Elements, std::optional<std::string_view> Name = std::nullopt)
        : RecordType(RTy_union, Elements, Name) {}
};

} // namespace scc

#endif // SCC_AST_RECORDTYPE_H
