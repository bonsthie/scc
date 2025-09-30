#include "scc/Token/Token.h"
#include <unordered_map>

using namespace scc;

#undef KEYWORD_TOKENKIND
static std::unordered_map<std::string, tok::TokenKind> TokenMap{
#define KEYWORD_TOKENKIND(id, string) {string, tok::id},
#include "scc/Token/TokenKinds.def"
};

void create_keyword_token(Token &CurTok, std::string &&Word) {
    auto KeyWord = TokenMap.find(Word);
    if (KeyWord != TokenMap.end()) {
        CurTok.setTokenKind(KeyWord->second);
        return;
    }
    CurTok.setTokenKind(tok::identifier);
    CurTok.setValue(std::move(Word));
}
