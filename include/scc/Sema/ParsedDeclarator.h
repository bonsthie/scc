#ifndef SCC_SEMA_PARSEDDECLARATOR_H
#define SCC_SEMA_PARSEDDECLARATOR_H

#include "scc/ADT/vector.h"
#include "scc/AST/Qualifiers.h"
#include "scc/FileManager/MemoryBufferView.h"
#include <iostream>
#include <string_view>
#include <utility>
#include <variant>

namespace scc {

// tempoary need to be put in AST
class Expr {
  public:
    int Num;
};

struct DeclPointerChunk {
    Qualifiers Qual;
};

struct DeclArrayChunk {
    Expr Size;
};

struct DeclFuncChunk {
    Expr FuncArgsDecl;
};

using DeclSuffixType = std::variant<DeclArrayChunk, DeclFuncChunk>;

using DeclaratorChunksType = std::variant<DeclPointerChunk, DeclSuffixType>;

struct DeclaratorChunks {
    DeclaratorChunksType Type;
    SourceRange          Range;

    DeclaratorChunks(DeclaratorChunksType &&Type, SourceRange Range) : Type(Type), Range(Range) {}
};

// tempory version to setup sema
class ParsedDeclarator {
    std::string_view                      Name;
    SourceRange                           NameSourceRange;
    scc::SmallVector<DeclaratorChunks, 8> Chunks;

  public:
    ParsedDeclarator() = default;

    // TODO move the handeling to SmallVector Class
    ParsedDeclarator(ParsedDeclarator &&Other)
        : Name(Other.Name),
          NameSourceRange(Other.NameSourceRange) {
        // The inline allocator cannot move; move elements into our own storage.
        Chunks.reserve(Other.Chunks.size());
        for (auto &Chunk : Other.Chunks)
            Chunks.pushBack(std::move(Chunk));
        Other.Chunks.clear();
    }

    std::string_view getName() const { return Name; }

    const SourceRange &getNameSourceRange() const { return NameSourceRange; }
    SourceRange       &getNameSourceRange() { return NameSourceRange; }

    void setName(const std::string_view &NewName, SourceRange Range) {
        Name = NewName;
        NameSourceRange = Range;
    }

    void pushChunk(DeclaratorChunks &C) { Chunks.pushBack(C); }

    void dump(std::ostream &O = std::cerr) const;
};

} // namespace scc

#endif // SCC_SEMA_PARSEDDECLARATOR_H
