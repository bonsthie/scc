#ifndef SCC_LEX_FILELEXER_H
#define SCC_LEX_FILELEXER_H

#include "scc/Error/ErrorManager.h"
#include "scc/FileManager/File.h"
#include "scc/FileManager/MemoryBufferView.h"
#include "scc/Frontend/LangOpt.h"
#include "scc/Lex/SizedChar.h"
#include "scc/String/StringInterner.h"
#include "scc/Token/Token.h"
#include "scc/Token/TokenStream.h"
namespace scc {

class FileLexer : public TokenStream {
    ErrorManager    &EM;
    const LangOpt   &Opts;
    StringInterner  &SI;
    MemoryBufferView MemBufferView;
    FileID           FID;
    MemoryViewPos    Pos;

    Token LastToken;
    Token NextToken; // if cached with peak

    bool ParseDirtyToken = false;

  public:
    FileLexer(File &F, StringInterner &SI, ErrorManager &EM)
        : FileLexer(F, SI, EM, defaultLangOpt()) {}

    FileLexer(File &F, StringInterner &SI, ErrorManager &EM, const LangOpt &Opts)
        : EM(EM),
          Opts(Opts),
          SI(SI),
          MemBufferView(F.view()),
          FID(F.getFileID()) {}

    FileLexer(MemoryBufferView &&MBF, StringInterner &SI, FileID &FID, ErrorManager &EM)
        : FileLexer(std::move(MBF), SI, FID, EM, defaultLangOpt()) {}

    FileLexer(MemoryBufferView &&MBF, StringInterner &SI, FileID &FID, ErrorManager &EM,
              const LangOpt &Opts)
        : EM(EM),
          Opts(Opts),
          SI(SI),
          MemBufferView(MBF),
          FID(FID) {}

    bool next(Token &CurTok);
    bool nextRaw(Token &CurTok);

    bool lexInclude(Token &CurTok);

    const FileID &getFID() const { return FID; }
    const LangOpt &getLangOpt() const { return Opts; }

  private:
    static const LangOpt &defaultLangOpt();

    SizedChar getChar(void);
    SizedChar peakChar(int Idx = 1);
    SizedChar peakCharAtIdx(int Idx = 0);
    SizedChar decodeLogicalChar(const char *Ptr);
    void      consumeChar(void);
    void      consumeChar(SizedChar sc);
    void      consumeChar(int size);

    template <typename... Args>
    bool consumeIfIs(int c, Args... chars) {
        int    expected[] = {c, chars...};
        size_t offset = 0;
        size_t total = 0;

        for (int ch : expected) {
            SizedChar sc = peakCharAtIdx(offset);
            if (sc.value != ch)
                return false;
            if (sc.size > 1)
                ParseDirtyToken = true;
            offset += sc.size;
            total += sc.size;
        }

        consumeChar(total);
        return true;
    }

    // return true if you found a eof before the char
    bool consumeCharUntil(int c);
    bool consumeCharUntil(int c, std::string &Str);

    // return the true and consume it only if equal to c
    bool ConsumeCharIfEqual(int c);

    inline int getCharIfOneOf(int c) { return ConsumeCharIfEqual(c) ? c : 0; }

    template <typename... Args>
    int getCharIfOneOf(int c, Args... Next) {
        if (ConsumeCharIfEqual(c))
            return c;
        return getCharIfOneOf(Next...);
    }

    bool handleSpaceToken(Token &CurTok, SizedChar LastChar);
    bool handleNumToken(Token &CurTok, SizedChar LastChar);
    bool handleKeyword(Token &CurTok, SizedChar LastChar);
    bool handleString(Token &CurTok, SizedChar LastChar);
    bool handleChar(Token &CurTok, SizedChar LastChar);

    inline size_t LexSign(Token &CurTok, SizedChar LastChar);

    std::string_view makeStringView(const MemoryViewPos &Begin, const MemoryViewPos &End) const;
    void             setTokenValue(Token &CurTok, const MemoryViewPos &End);
    void             setTokenValue(Token &CurTok);
};

} // namespace scc

#endif // SCC_LEX_FILELEXER_H
