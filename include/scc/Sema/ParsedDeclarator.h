#ifndef SCC_SEMA_PARSEDDECLARATOR_H
#define SCC_SEMA_PARSEDDECLARATOR_H

#include "scc/FileManager/MemoryBufferView.h"
#include <string_view>

namespace scc {

// tempory version to setup sema
class ParsedDeclarator {
    std::string_view Name;
    SourceRange      NameSourceRange;

  public:
    ParsedDeclarator() = default;

    std::string_view getName() const { return Name; }

    const SourceRange &getNameSourceRange() const { return NameSourceRange; }
    SourceRange       &getNameSourceRange() { return NameSourceRange; }

    void setName(const std::string_view &NewName, SourceRange Range) {
        Name = NewName;
        NameSourceRange = Range;
    }
};

} // namespace scc

#endif // SCC_SEMA_PARSEDDECLARATOR_H
