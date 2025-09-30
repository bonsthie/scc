#ifndef SCC_LEX_FILELEXER_H
#define SCC_LEX_FILELEXER_H

#include "scc/FileManager/File.h"
#include "scc/FileManager/MemoryBufferView.h"
#include "scc/Token/Token.h"
#include "scc/Token/TokenStream.h"
namespace scc {

class FileLexer : public TokenStream {
    MemoryBufferView MemBufferView;
    unsigned         BuffPos = 0;
    unsigned         LinePos = 0;
    unsigned         ColumnPos = 0;

    Token LastToken;
    Token NextToken; // if cached with peak

  public:
    FileLexer(File &F) : MemBufferView(F.view()) {}

    bool next(Token &CurTok);
    bool nextRaw(Token &CurTok);

    bool parseRaw(Token &CurTok);

    bool ParseInclue(Token &CurTok);

  private:
    int getChar(void);
    int peakChar(void);
    int peakChar(int Idx);
	void consumeChar(void);

    bool handleSpaceToken(Token &CurTok, int LastChar);
    bool handleNumToken(Token &CurTok, int LastChar);
    bool handleKeyword(Token &CurTok, int LastChar);

    inline size_t LexSign(Token &CurTok, int LastChar);
};

} // namespace scc

#endif // SCC_LEX_FILELEXER_H
