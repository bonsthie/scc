#ifndef SCC_LEX_FILELEXER_H
#define SCC_LEX_FILELEXER_H

#include "scc/Error/ErrorManager.h"
#include "scc/FileManager/File.h"
#include "scc/FileManager/MemoryBufferView.h"
#include "scc/Lex/SizedChar.h"
#include "scc/Token/Token.h"
#include "scc/Token/TokenStream.h"
namespace scc {

class FileLexer : public TokenStream {
    ErrorManager    &EM;
    MemoryBufferView MemBufferView;
    FileID           FID;
    Token::Pos       Pos;

    Token LastToken;
    Token NextToken; // if cached with peak

  public:
    FileLexer(File &F, ErrorManager &EM) : EM(EM), MemBufferView(F.view()), FID(F.getFileID()) {}
    FileLexer(MemoryBufferView &&MBF, FileID &FID, ErrorManager &EM)
        : EM(EM),
          MemBufferView(MBF),
          FID(FID) {}

    bool next(Token &CurTok);
    bool nextRaw(Token &CurTok);

    bool lexInclude(Token &CurTok);

    const FileID &getFID() const { return FID; }

  private:
    SizedChar getChar(void);
    SizedChar peakChar(int Idx = 1);
    SizedChar peakCharAtIdx(int Idx = 0);
    void      consumeChar(void);
    void      consumeChar(SizedChar sc);
    void      consumeChar(int size);

    template <typename... Args> bool consumeIfIs(int c, Args... chars) {
        int    expected[] = {c, chars...};
        size_t offset = 0;
        size_t total = 0;

        for (int ch : expected) {
            SizedChar sc = peakCharAtIdx(offset);
            if (sc.value != ch)
                return false;
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

    template <typename... Args> int getCharIfOneOf(int c, Args... Next) {
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
};

} // namespace scc

#endif // SCC_LEX_FILELEXER_H
