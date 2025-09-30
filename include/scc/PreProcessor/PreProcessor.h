#ifndef SCC_PREPROCESSOR_PREPROCESSOR_H
#define SCC_PREPROCESSOR_PREPROCESSOR_H

#include "scc/FileManager/File.h"
#include "scc/Lex/FileLexer.h"
#include "scc/Token/TokenStream.h"
#include <memory>
#include <vector>

// class DefineManager;
// class IfdefManager;

namespace scc {

class PreProcessor {
    std::vector<std::unique_ptr<TokenStream>> TSList;
    std::vector<FileID>                       FileScope;

    // DefineManager                             DM;
    // IfdefManager                              ifdefManager;
    // LineMapper								 Mapper;

  public:

    PreProcessor(File &F);

    bool next(Token &Tok);

  private:
    bool handlePP(Token &Tok);

    // TokenStream list utils
    void addNewTokenStream(File &F) { TSList.push_back(std::make_unique<FileLexer>(F)); }
};
} // namespace scc

#endif // SCC_PREPROCESSOR_PREPROCESSOR_H
