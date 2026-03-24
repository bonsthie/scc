#include "scc/Token/Token.h"
#include "scc/Lex/DecodeChar.h"
#include <iomanip>
#include <sstream>
#include <unordered_map>

using namespace scc;

static std::unordered_map<std::string_view, tok::TokenKind> TokenMap{
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

void scc::create_keyword_token(Token &CurTok) {
    auto KeyWord = TokenMap.find(CurTok.getValue());
    if (KeyWord != TokenMap.end()) {
        CurTok.setTokenKind(KeyWord->second);
        return;
    }
    CurTok.setTokenKind(tok::identifier);
}

std::string scc::clean_token(std::string_view str) {
    std::string clean;
    clean.reserve(str.size());

    const char *Ptr = str.data();
    const char *End = str.data() + str.size();

    while (Ptr < End) {
        SizedChar sc = decode_logical_char(Ptr, End);
        clean.push_back(sc.value);
        Ptr += sc.size;
    }

    return clean;
}

void Token::print(std::ostream &OS) const {
    std::ostringstream KindFormat;
    KindFormat << stringify_token_kind(TKind);

    if (!Value.empty()) {
        KindFormat << " '" << Value << "'";
    }

    OS << std::left << std::setw(30) << KindFormat.str();

    if (isStartOfLine())
        OS << " [StartOfLine]";

    if (isDirty())
        OS << " [Unclean='" << DirtyValue << "']";

    OS << " Loc=<" << posViewBegin() << '>';
}

std::ostream &scc::operator<<(std::ostream &OS, const Token &T) {
    T.print(OS);
    return OS;
}
