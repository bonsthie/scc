#ifndef SCC_PREPROCESSOR_PREPROCESSOR_H
#define SCC_PREPROCESSOR_PREPROCESSOR_H

#include "scc/ADT/vector.h"
#include "scc/Error/ErrorManager.h"
#include "scc/FileManager/File.h"
#include "scc/FileManager/FileManager.h"
#include "scc/Lex/FileLexer.h"
#include "scc/String/StringInterner.h"
#include "scc/Token/Token.h"
#include "scc/Token/TokenStream.h"
#include <memory>

// class DefineManager;
// class IfdefManager;

namespace scc {

class PreProcessor {
    scc::vector<std::unique_ptr<TokenStream>> TSList;
    TokenStream                              *CurrentTokStream = nullptr;

    ErrorManager   &EM;
    FileManager    &FM;
    StringInterner &SI;

    // DefineManager                             DM;
    // IfdefManager                              ifdefManager;
    // LineMapper								 Mapper;

  public:
    PreProcessor(File &F, ErrorManager &EM, FileManager &FM, StringInterner &SI);

    bool next(Token &Tok);
    bool nextRaw(Token &Tok);

  private:
    bool handlePP(Token &Tok);
    bool handleInclude(Token &Tok, FileLexer &FL);

    TokenStream *addNewTokenStream(File &F) {
        TSList.emplace_back(std::make_unique<FileLexer>(F, SI, EM));
        return (CurrentTokStream = TSList.back().get());
    }

    TokenStream *getNextTokenStream() {
        if (TSList.size() == 0)
            return nullptr;
        return TSList.back().get();
    }

    void popTokenStream() {
        TSList.pop_back();
        CurrentTokStream = nullptr;
    }
};
} // namespace scc

#endif // SCC_PREPROCESSOR_PREPROCESSOR_H
