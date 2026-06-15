#ifndef SCC_LEX_FILELEXER_H
#define SCC_LEX_FILELEXER_H

#include "scc/FileManager/File.h"
#include "scc/FileManager/MemoryBufferView.h"
#include "scc/Frontend/FrontendErrorManager.h"
#include "scc/Frontend/LangOpt.h"
#include "scc/Lex/SizedChar.h"
#include "scc/String/StringInterner.h"
#include "scc/Token/Token.h"
#include "scc/Token/TokenStream.h"
namespace scc {

class FileLexer : public TokenStream {
    FrontendErrorManager &EM;
    const LangOpt   &Opts;
    StringInterner  &SI;
    MemoryBufferView MemBufferView;
    FileID           FID;
    MemoryViewPos    Pos;

    Token LastToken;
    Token NextToken; // if cached with peak

    bool ParseDirtyToken = false;

  public:
    FileLexer(File &F, StringInterner &SI, FrontendErrorManager &EM)
        : FileLexer(F, SI, EM, defaultLangOpt()) {}

    FileLexer(File &F, StringInterner &SI, FrontendErrorManager &EM, const LangOpt &Opts)
        : EM(EM),
          Opts(Opts),
          SI(SI),
          MemBufferView(F.view()),
          FID(F.getFileID()) {}

    FileLexer(MemoryBufferView &&MBF, StringInterner &SI, FileID &FID, FrontendErrorManager &EM)
        : FileLexer(std::move(MBF), SI, FID, EM, defaultLangOpt()) {}

    FileLexer(MemoryBufferView &&MBF, StringInterner &SI, FileID &FID, FrontendErrorManager &EM,
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
    SizedChar decodeLogicalChar(const char *Ptr, bool EmitTrigraphWarning = false);
    SizedChar handleTrigraph(const char *Ptr, bool EmitWarning);
    void      reportTrigraphWarning(const char *Ptr, int TrigraphValue, bool Converted);
    void      consumeChar(void);
    void      consumeChar(SizedChar Sc);
    void      consumeChar(int Size);

    template <typename... Args>
    bool consumeIfIs(int c, Args... Chars) {
        int    Expected[] = {c, Chars...};
        size_t Offset = 0;
        size_t Total = 0;

        for (int Ch : Expected) {
            SizedChar Sc = peakCharAtIdx(Offset);
            if (Sc.Value != Ch)
                return false;
            if (Sc.Size > 1)
                ParseDirtyToken = true;
            Offset += Sc.Size;
            Total += Sc.Size;
        }

        consumeChar(Total);
        return true;
    }

    // return true if you found a eof before the char
    bool consumeCharUntil(int c);
    bool consumeCharUntil(int c, std::string &Str);

    // return the true and consume it only if equal to c
    bool consumeCharIfEqual(int c);

    inline int getCharIfOneOf(int c) { return consumeCharIfEqual(c) ? c : 0; }

    template <typename... Args>
    int getCharIfOneOf(int c, Args... Next) {
        if (consumeCharIfEqual(c))
            return c;
        return getCharIfOneOf(Next...);
    }

    bool handleSpaceToken(Token &CurTok, SizedChar LastChar);
    bool handleNumToken(Token &CurTok, SizedChar LastChar);
    bool handleKeyword(Token &CurTok, SizedChar LastChar);
    bool handleString(Token &CurTok, SizedChar LastChar);
    bool handleChar(Token &CurTok, SizedChar LastChar);

    inline size_t lexSign(Token &CurTok, SizedChar LastChar);

    std::string_view makeStringView(const MemoryViewPos &Begin, const MemoryViewPos &End) const;
    void             setTokenValue(Token &CurTok, const MemoryViewPos &End);
    void             setTokenValue(Token &CurTok);
};

} // namespace scc

#endif // SCC_LEX_FILELEXER_H
