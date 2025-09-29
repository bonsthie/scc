#ifndef SCC_LEX_FILELEXER_H
#define SCC_LEX_FILELEXER_H

#include "scc/FileManager/File.h"
#include "scc/FileManager/MemoryBufferView.h"
#include "scc/Token/TokenStream.h"
namespace scc {

class FileLexer : TokenStream {
    MemoryBufferView MemBufferView;

  public:
    FileLexer(File &F) : MemBufferView(F.view()) {}

	Token next() {}

	Token parseRaw() {}

	Token ParseInclue() {}
};

} // namespace scc

#endif // SCC_LEX_FILELEXER_H
