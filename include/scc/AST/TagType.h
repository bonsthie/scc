#ifndef SCC_AST_TAGTYPE_H
#define SCC_AST_TAGTYPE_H

#include "scc/AST/TagDecl.h"
#include "scc/AST/Type.h"

namespace scc {

class TagType : public Type {
    TagDecl *Decl = nullptr;

  protected:
    TagType(TypeKind Kind, TagDecl *Decl) : Type(Kind), Decl(Decl) {}

  public:
    TagDecl       *getDecl() { return Decl; }
    const TagDecl *getDecl() const { return Decl; }
    void           setDecl(TagDecl *NewDecl) { Decl = NewDecl; }

    bool isTagType() const { return true; }
    bool isAnonymous() const { return !Decl || Decl->isAnonymous(); }
    bool isEnum() const { return Decl && Decl->isEnum(); }
    bool isStruct() const { return Decl && Decl->isStruct(); }
    bool isUnion() const { return Decl && Decl->isUnion(); }
    bool isRecord() const { return Decl && Decl->isRecord(); }
};

class EnumType : public TagType {
  public:
    explicit EnumType(EnumDecl *Decl) : TagType(TypeKind::Enum, Decl) {}

    EnumDecl       *getDecl() { return static_cast<EnumDecl *>(TagType::getDecl()); }
    const EnumDecl *getDecl() const { return static_cast<const EnumDecl *>(TagType::getDecl()); }
    void            setDecl(EnumDecl *NewDecl) { TagType::setDecl(NewDecl); }
};

class RecordType : public TagType {
  public:
    explicit RecordType(RecordDecl *Decl) : TagType(TypeKind::Record, Decl) {}

    RecordDecl       *getDecl() { return static_cast<RecordDecl *>(TagType::getDecl()); }
    const RecordDecl *getDecl() const {
        return static_cast<const RecordDecl *>(TagType::getDecl());
    }
    void setDecl(RecordDecl *NewDecl) { TagType::setDecl(NewDecl); }
};

} // namespace scc

#endif // SCC_AST_TAGTYPE_H
