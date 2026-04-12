#ifndef SCC_PREPROCESSOR_PREPROCESSOR_H
#define SCC_PREPROCESSOR_PREPROCESSOR_H

#include "scc/ADT/vector.h"
#include "scc/FileManager/File.h"
#include "scc/FileManager/FileManager.h"
#include "scc/Frontend/FrontendErrorManager.h"
#include "scc/Frontend/LangOpt.h"
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

    FrontendErrorManager &EM;
    FileManager    &FM;
    const LangOpt  &Opts;
    StringInterner &SI;

    // DefineManager                             DM;
    // IfdefManager                              ifdefManager;
    // LineMapper								 Mapper;

  public:
    PreProcessor(File &F, FrontendErrorManager &EM, FileManager &FM, StringInterner &SI)
        : PreProcessor(F, EM, FM, SI, defaultLangOpt()) {}

    PreProcessor(File &F, FrontendErrorManager &EM, FileManager &FM, StringInterner &SI,
                 const LangOpt &Opts);

    bool next(Token &Tok);
    bool nextRaw(Token &Tok);
    const LangOpt &getLangOpt() const { return Opts; }

  private:
    static const LangOpt &defaultLangOpt();

    bool handlePP(Token &Tok);
    bool handleInclude(Token &Tok, FileLexer &FL);

    TokenStream *addNewTokenStream(File &F) {
        TSList.emplace_back(std::make_unique<FileLexer>(F, SI, EM, Opts));
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
