#include "scc/Token/Token.h"
#include "scc/FileManager/FileID.h"
#include <iomanip>
#include <ios>
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

void Token::print(std::ostream &OS) const {
    std::string kindStr = stringify_token_kind(TKind);
    kindStr += Value.empty() ? "" : (" '" + Value + "'");

    OS << std::left << std::setw(30) << kindStr;

    // Print flag
    if (isStartOfLine())
        OS << " [StartOfLine]";

    std::ostringstream LocStr;
    LocStr << "Loc=<" << posViewBegin() << '>';
    std::string loc = LocStr.str();

    OS << " " << loc;
}

std::ostream &scc::operator<<(std::ostream &OS, const Token &T) {
    T.print(OS);
    return OS;
}

void Token::PosView::print(std::ostream &OS) const {
    OS << (FID ? FID->getBaseName() : "<stdin>") << ':' << P.Line << ':' << P.Column;
}

std::ostream &scc::operator<<(std::ostream &OS, const Token::PosView &V) {
    V.print(OS);
    return OS;
}
