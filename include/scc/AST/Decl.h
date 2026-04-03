#ifndef SCC_AST_DECL_H
#define SCC_AST_DECL_H

namespace scc {

enum DeclKind {
    None,
    Type,
    Tag,
    Typedef,
    RecordField,
    EnumField,
};

class Decl {
    DeclKind Kind;

  public:
    Decl(DeclKind Kind) : Kind(Kind) {}

    DeclKind kind() const { return Kind; }
};

} // namespace scc

#endif // SCC_AST_DECL_H
