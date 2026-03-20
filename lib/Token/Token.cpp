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

void scc::create_keyword_token(Token &CurTok, std::string_view Word) {
    auto KeyWord = TokenMap.find(Word);
    if (KeyWord != TokenMap.end()) {
        CurTok.setTokenKind(KeyWord->second);
        return;
    }
    CurTok.setTokenKind(tok::identifier);
    CurTok.setValue(Word);
}

std::string clean_token(std::string_view str) {
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

std::string Token::getCleanValue() const { return clean_token(Value); }

void Token::print(std::ostream &OS) const {
    std::ostringstream KindFormat;
    KindFormat << stringify_token_kind(TKind);

    if (!Value.empty()) {
        std::string v = isDirty() ? getCleanValue() : std::string(Value);
        KindFormat << " '" << v << "'";
    }

    OS << std::left << std::setw(30) << KindFormat.str();

    if (isStartOfLine())
        OS << " [StartOfLine]";

    if (isDirty())
        OS << " [Unclean='" << Value << "']";

    OS << " Loc=<" << posViewBegin() << '>';
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
