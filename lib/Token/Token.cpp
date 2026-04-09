#include "scc/Colors/Colors.h"
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

static const char *kind_color(tok::TokenKind Kind) {
    switch (Kind) {
    case tok::unknown:
        return Color::red();
    case tok::identifier:
        return Color::cyan();
    case tok::numeric_constant:
    case tok::char_constant:
    case tok::string_literal:
    case tok::system_string:
        return Color::yellow();
    case tok::comment:
    case tok::comment_line:
    case tok::space:
    case tok::eol:
    case tok::eof:
        return Color::dim();

#define DELIMITERS(id, string) case tok::id:
#define OPERATORS(id, string) case tok::id:
        return Color::magenta();

#define KEYWORD_TOKENKIND(id, string) case tok::id:
        return Color::blue();

#define PP_TOKENKIND(id, string) case tok::id:
        return Color::red();

#include "scc/Token/TokenKinds.def"

    default:
        return "";
    }
}

static const char *value_color(tok::TokenKind Kind) {
    switch (Kind) {
    case tok::identifier:
        return Color::cyan();
    case tok::numeric_constant:
    case tok::char_constant:
    case tok::string_literal:
    case tok::system_string:
        return Color::yellow();
    default:
        return Color::green();
    }
}

#ifdef __clang__
#pragma clang diagnostic ignored "-Wc99-designator"
#endif
std::string scc::stringify_token_kind(tok::TokenKind Kind) {
    static const char *str[] = {
#define TOK(id) [tok::id] = #id,
#include "scc/Token/TokenKinds.def"

    };

    return std::string(str[Kind]);
}

std::string scc::token_spelling(tok::TokenKind Kind) {
    static const char *str[] = {
#define TOK(id) [tok::id] = #id,
#define TOKENKIND(id, string) [tok::id] = string,
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
    const std::string KindText = stringify_token_kind(TKind);
    std::string       PlainField = KindText;
    if (!Value.empty())
        PlainField += " '" + std::string(Value) + "'";

    if (Color::enabled()) {
        OS << kind_color(TKind) << KindText << Color::reset();
        if (!Value.empty())
            OS << " '" << value_color(TKind) << Value << Color::reset() << "'";

        if (PlainField.size() < 30)
            OS << std::string(30 - PlainField.size(), ' ');
    } else {
        OS << std::left << std::setw(30) << PlainField;
    }

    if (isStartOfLine())
        OS << Color::dim() << " [StartOfLine]" << Color::reset();

    if (isDirty())
        OS << Color::yellow() << " [Unclean='" << DirtyValue << "']" << Color::reset();

    OS << Color::dim() << " Loc=<" << posViewBegin() << '>' << Color::reset();
}

std::ostream &scc::operator<<(std::ostream &OS, const Token &T) {
    T.print(OS);
    return OS;
}
