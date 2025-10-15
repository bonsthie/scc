#include "scc/Token/Token.h"
#include <unordered_map>

using namespace scc;

static std::unordered_map<std::string, tok::TokenKind> TokenMap{
#define KEYWORD_TOKENKIND(id, string) {string, tok::id},
#include "scc/Token/TokenKinds.def"
};

#pragma clang diagnostic ignored "-Wc99-designator"
std::string scc::stringify_token_kind(tok::TokenKind Kind) {
    static const char *str[] = {
#define TOK(id) [tok::id] = #id,
#include "scc/Token/TokenKinds.def"

    };

    return std::string(str[Kind]);
}

void scc::create_keyword_token(Token &CurTok, std::string &&Word) {
    auto KeyWord = TokenMap.find(Word);
    if (KeyWord != TokenMap.end()) {
        CurTok.setTokenKind(KeyWord->second);
        return;
    }
    CurTok.setTokenKind(tok::identifier);
    CurTok.setValue(std::move(Word));
}
