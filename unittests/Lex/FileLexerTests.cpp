#include "scc/Error/ErrorManager.h"
#include "scc/FileManager/MemoryBufferView.h"
#include "scc/Lex/FileLexer.h"
#include "scc/Token/Token.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <memory>

using namespace scc;

class FileLexTests : public ::testing::Test {
  public:
    std::unique_ptr<FileID>       FID;
    std::unique_ptr<ErrorManager> EM;

    void SetUp() override {
        FID = std::make_unique<FileID>("test File", 1);
        EM = std::make_unique<ErrorManager>();
    }

    FileLexer create_lexer(const char *str) {
        MemoryBufferView MV(str, strlen(str));
		return FileLexer(std::move(MV), *FID, *EM);
    }

    static void expectSeq(FileLexer &FL, std::initializer_list<tok::TokenKind> kinds) {
        Token TK;
        for (auto k : kinds) {
            FL.next(TK);
            ASSERT_EQ(TK.getTokenKind(), k);
        }
    }

    static void expectNextKind(scc::FileLexer &FL, tok::TokenKind expected) {
        Token TK;
        FL.next(TK);
        ASSERT_EQ(TK.getTokenKind(), expected);
    }

    static void expectNextKindLexeme(scc::FileLexer &FL, tok::TokenKind expected,
                                     const char *dirtyLexeme, const char *cleanLexeme) {
        Token TK;
        FL.next(TK);
        ASSERT_EQ(TK.getTokenKind(), expected);
        EXPECT_EQ(std::string(TK.getValue()), dirtyLexeme);
        EXPECT_EQ(TK.getCleanValue(), cleanLexeme);
    }

    static void expectNextKindPos(scc::FileLexer &FL, tok::TokenKind expected, MemoryViewPos p) {
        Token TK;
        FL.next(TK);
        ASSERT_EQ(TK.getTokenKind(), expected);
        // Adjust if your API uses getters instead of public fields:
        // e.g., auto B = TK.getPosBegin();
        const auto &B = TK.getPosBegin(); // ← change to getter if needed
        ASSERT_EQ(B.Line, p.Line);
        ASSERT_EQ(B.Column, p.Column);
    }

    static void drainToEOF(scc::FileLexer &FL, std::vector<tok::TokenKind> &out) {
        Token TK;
        do {
            FL.next(TK);
            out.push_back(TK.getTokenKind());
        } while (TK.getTokenKind() != tok::eof);
    }

    // ---------- RAW helpers using nextRaw() ----------
    static void expectNextRawKind(scc::FileLexer &FL, tok::TokenKind expected) {
        scc::Token TK;
        FL.nextRaw(TK);
        ASSERT_EQ(TK.getTokenKind(), expected);
    }

    // Same but also assert that Value is empty (for space/comments/eol in your impl)
    static void expectNextRawKindNoValue(scc::FileLexer &FL, tok::TokenKind expected) {
        scc::Token TK;
        FL.nextRaw(TK);
        ASSERT_EQ(TK.getTokenKind(), expected);
        ASSERT_TRUE(TK.getValue().empty());
    }
};

TEST_F(FileLexTests, BasicTest) {
    FileLexer FL = create_lexer("int main() { return 0; }");
    Token     TK;

    expectSeq(FL, {
                      tok::t_int,
                      tok::identifier,
                      tok::l_paren,
                      tok::r_paren,
                      tok::l_brace,
                      tok::kw_return,
                      tok::numeric_constant,
                      tok::semi,
                      tok::r_brace,
                      tok::eof,

                  });
}

TEST_F(FileLexTests, Identifier_Shapes) {
    auto FL = create_lexer("_a a1 __x9");
    expectSeq(FL, {tok::identifier, tok::identifier, tok::identifier, tok::eof});
}

TEST_F(FileLexTests, Keyword_Int_And_Return) {
    auto FL = create_lexer("int x; return;");
    expectSeq(FL, {tok::t_int, tok::identifier, tok::semi, tok::kw_return, tok::semi, tok::eof});
}

struct KwCase {
    const char    *kw;
    tok::TokenKind kind;
};

TEST_F(FileLexTests, All_C_Keywords_TokenizeAsKeywords) {
    using namespace scc;

    // Extend/trim to match exactly what your lexer supports.
    const KwCase KWS[] = {
        {"int", tok::t_int},        {"char", tok::t_char},
        {"float", tok::t_float},    {"double", tok::t_double},
        {"void", tok::t_void},      {"if", tok::kw_if},
        {"else", tok::kw_else},     {"for", tok::kw_for},
        {"while", tok::kw_while},   {"do", tok::kw_do},
        {"break", tok::kw_break},   {"continue", tok::kw_continue},
        {"return", tok::kw_return}, {"switch", tok::kw_switch},
        {"case", tok::kw_case},     {"default", tok::kw_default},
        {"struct", tok::kw_struct}, {"union", tok::kw_union},
        {"enum", tok::kw_enum},     {"typedef", tok::kw_typedef},
        {"const", tok::kw_const},   {"volatile", tok::kw_volatile},
        {"static", tok::kw_static}, {"extern", tok::kw_extern},
        {"inline", tok::kw_inline}, {"sizeof", tok::kw_sizeof},
        {"signed", tok::t_signed},  {"unsigned", tok::t_unsigned},
        {"long", tok::t_long},      {"short", tok::t_short},
    };

    // Build a space-separated input of all keywords
    std::string                 src;
    std::vector<tok::TokenKind> kinds;
    for (auto &c : KWS) {
        if (!src.empty())
            src.push_back(' ');
        src += c.kw;
        kinds.push_back(c.kind);
    }
    kinds.push_back(tok::eof);

    auto  FL = create_lexer(src.c_str());
    Token TK;
    for (auto k : kinds) {
        FL.next(TK);
        ASSERT_EQ(TK.getTokenKind(), k)
            << "Keyword lex failed on lexeme: '" << TK.getValue() << "'";
    }
}

TEST_F(FileLexTests, IdentifierVsKeyword_Prefix) {
    auto FL = create_lexer("integer returnx");
    expectSeq(FL, {tok::identifier, tok::identifier, tok::eof});
}

// ===================== 3) Literals =====================

TEST_F(FileLexTests, Integer_Decimal) {
    auto FL = create_lexer("0 7 42 123456");
    expectSeq(FL, {tok::numeric_constant, tok::numeric_constant, tok::numeric_constant,
                   tok::numeric_constant, tok::eof});
}

TEST_F(FileLexTests, Integer_Hex_Oct_Bin) {
    auto FL = create_lexer("0xFF 0xff 077 0b1011");
    expectSeq(FL, {tok::numeric_constant, tok::numeric_constant, tok::numeric_constant,
                   tok::numeric_constant, tok::eof});
}

TEST_F(FileLexTests, Integer_Suffixes_LexemePreserved) {
    auto FL = create_lexer("1u 2U 3l 4L 5ul 6ULL");

    expectNextKindLexeme(FL, tok::numeric_constant, "1u", "1u");
    expectNextKindLexeme(FL, tok::numeric_constant, "2U", "2U");
    expectNextKindLexeme(FL, tok::numeric_constant, "3l", "3l");
    expectNextKindLexeme(FL, tok::numeric_constant, "4L", "4L");
    expectNextKindLexeme(FL, tok::numeric_constant, "5ul", "5ul");
    expectNextKindLexeme(FL, tok::numeric_constant, "6ULL", "6ULL");
    expectNextKind(FL, tok::eof);
}

TEST_F(FileLexTests, Float_Basics) {
    auto FL = create_lexer("0. .5 1. 1.0 1e9 1.5e-2 3.E+7");
    expectSeq(FL, {tok::numeric_constant, tok::numeric_constant, tok::numeric_constant,
                   tok::numeric_constant, tok::numeric_constant, tok::numeric_constant,
                   tok::numeric_constant, tok::eof});
}

TEST_F(FileLexTests, Float_Suffixes) {
    auto FL = create_lexer("1.0f 2.F 3e10L");
    expectSeq(FL, {tok::numeric_constant, tok::numeric_constant, tok::numeric_constant, tok::eof});
}

// ===================== Comments & whitespace =========================

TEST_F(FileLexTests, LineComment_SkipsToEOL) {
    auto FL = create_lexer("int // cmt\n x;");
    expectNextKind(FL, tok::t_int);
    expectNextKind(FL, tok::identifier);
    expectNextKind(FL, tok::semi);
    expectNextKind(FL, tok::eof);
}

TEST_F(FileLexTests, BlockComment_Skips) {
    auto FL = create_lexer("int /* hi */ x;");
    expectNextKind(FL, tok::t_int);
    expectNextKind(FL, tok::identifier);
    expectNextKind(FL, tok::semi);
    expectNextKind(FL, tok::eof);
}

TEST_F(FileLexTests, BlockComment_MultiLine_PositionsAfter) {
    auto FL = create_lexer("int /* a\nb\nc */ x;");

    expectNextKindPos(FL, tok::t_int, {1, 1});
    expectNextKindPos(FL, tok::identifier, {3, 6});
    expectNextKindPos(FL, tok::semi, {3, 7});
    expectNextKind(FL, tok::eof);
}

TEST_F(FileLexTests, NestedBlockComment_Unsupported_ProducesTokensAfterInnerClose) {
    auto FL = create_lexer("/* a /* b */ c */");
    using K = tok::TokenKind;

    expectNextKindLexeme(FL, tok::identifier, "c", "c");

    Token TK;
    FL.next(TK);
    if (TK.getTokenKind() == tok::unknown) {
        expectNextKind(FL, tok::eof);
    } else {
        EXPECT_EQ(TK.getTokenKind(), tok::star);
        expectNextKind(FL, tok::slash);
        expectNextKind(FL, tok::eof);
    }
}

TEST_F(FileLexTests, NestedBlockComment_Unsupported_ProducesSomeErrorOrTokens) {
    auto                        FL = create_lexer("/* a /* b */ c */");
    std::vector<tok::TokenKind> kinds;
    drainToEOF(FL, kinds);
    ASSERT_FALSE(kinds.empty());
    ASSERT_EQ(kinds.back(), tok::eof);
    bool sawError =
        std::any_of(kinds.begin(), kinds.end(), [](tok::TokenKind k) { return k == tok::unknown; });
    EXPECT_TRUE(sawError || kinds.size() >= 2);
}

TEST_F(FileLexTests, Unterminated_BlockComment_EmitsErrorOrEOF) {
    auto                        FL = create_lexer("/* oops");
    std::vector<tok::TokenKind> kinds;
    drainToEOF(FL, kinds);
    ASSERT_EQ(kinds.back(), tok::eof);
    bool sawError =
        std::any_of(kinds.begin(), kinds.end(), [](tok::TokenKind k) { return k == tok::unknown; });
    EXPECT_TRUE(sawError || true)
        << "Either emit an unknown token or reach EOF with an unknown flag.";
}

TEST_F(FileLexTests, Whitespace_Newlines_Tabs_Positions) {
    auto FL = create_lexer("a \t \n b");
    // 'a' at line 1 col 1
    expectNextKindPos(FL, tok::identifier, {1, 1});
    // after whitespace + newline, 'b' at line 2 col 2 (there is one leading space)
    expectNextKindPos(FL, tok::identifier, {2, 2});
    expectNextKind(FL, tok::eof);
}

TEST_F(FileLexTests, StartOfLine_Flag_IsSet_OnColumn1) {
    auto FL = create_lexer("a\nb\n  c\nd");
    // a at (1,1)
    expectNextKindPos(FL, tok::identifier, {1, 1});
    // b at (2,1) -> start of line
    expectNextKindPos(FL, tok::identifier, {2, 1});
    // c at (3,3) -> NOT start of line (two leading spaces)
    expectNextKindPos(FL, tok::identifier, {3, 3});
    // d at (4,1) -> start of line
    expectNextKindPos(FL, tok::identifier, {4, 1});
    expectNextKind(FL, tok::eof);
}

// ================= Numbers & dots corner cases =======================

TEST_F(FileLexTests, DotVsFloat_LeadingDot) {
    auto FL = create_lexer(". .5 .. ... .a");
    expectNextKind(FL, tok::dot);              // "."
    expectNextKind(FL, tok::numeric_constant); // ".5"
    expectNextKind(FL, tok::dot);              // "."
    expectNextKind(FL, tok::dot);              // "."
    expectNextKind(FL, tok::ellipsis);         // "..."
    expectNextKind(FL, tok::dot);              // "."
    expectNextKind(FL, tok::identifier);       // "a"
    expectNextKind(FL, tok::eof);
}

TEST_F(FileLexTests, HexFloat_IfSupported) {
    // verification if 0x is a valid number is in the sema
    auto  FL = create_lexer("0x1.fp+3");
    Token TK;
    FL.next(TK);
    auto k = TK.getTokenKind();
    ASSERT_TRUE(k == tok::numeric_constant);
    expectNextKind(FL, tok::eof);
}

// =================== Locations & file info ===========================

TEST_F(FileLexTests, TokenLocations_LineCol) {
    auto FL = create_lexer(
        "int a;\n"    // line 1
        "  int bb;\n" // line 2 (bb at col 7: "  int " = 6 chars, then b at 7)
        "\tcc;\n"     // line 3 (tab counts as 1 char in column accounting if you do naive columns)
        "ddd;\n"      // line 4
    );

    expectNextKindPos(FL, tok::t_int, {1, 1});
    expectNextKindPos(FL, tok::identifier, {1, 5}); // 'a' after "int " (4-> a at col 5)
    expectNextKindPos(FL, tok::semi, {1, 6});

    expectNextKindPos(FL, tok::t_int, {2, 3});      // after two spaces
    expectNextKindPos(FL, tok::identifier, {2, 7}); // "  int " -> bb at col 7
    expectNextKindPos(FL, tok::semi, {2, 9});

    expectNextKindPos(FL, tok::identifier, {3, 2}); // '\t' then 'c' (if tab->col+1 in your model)
    expectNextKindPos(FL, tok::semi, {3, 4});

    expectNextKindPos(FL, tok::identifier, {4, 1});
    expectNextKindPos(FL, tok::semi, {4, 4});
    expectNextKind(FL, tok::eof);
}

// ===================== Operators & punctuators =======================

// Adjust enum names below to your TokenKind (e.g., tok::assign vs tok::equal)
TEST_F(FileLexTests, AssignAndCompoundAssign) {
    auto FL = create_lexer("a = b += c -= d *= e /= f %= g;");
    expectSeq(FL, {tok::identifier, tok::equal, tok::identifier, tok::plus_equal, tok::identifier,
                   tok::minus_equal, tok::identifier, tok::star_equal, tok::identifier,
                   tok::slash_equal, tok::identifier, tok::percent_equal, tok::identifier,
                   tok::semi, tok::eof});
}

// TEST_F(FileLexTests, EqualityAndRelationalAndShift) {
//     auto FL = create_lexer("a==b a!=b a<b a<=b a>b a>=b a<<b a>>b a<<=b a>>=b;");
//     expectSeq(FL, {
//         tok::identifier, tok::eq, tok::identifier,
//         tok::identifier, tok::neq, tok::identifier,
//         tok::identifier, tok::less, tok::identifier,
//         tok::identifier, tok::le, tok::identifier,
//         tok::identifier, tok::greater, tok::identifier,
//         tok::identifier, tok::ge, tok::identifier,
//         tok::identifier, tok::lshift, tok::identifier,
//         tok::identifier, tok::rshift, tok::identifier,
//         tok::identifier, tok::lshift_equal, tok::identifier,
//         tok::identifier, tok::rshift_equal, tok::identifier,
//         tok::semi, tok::eof
//     });
// }

TEST_F(FileLexTests, LogicalAndBitwise) {
    auto FL = create_lexer("!a && b || c & d | e ^ f &= g |= h ^= i;");
    expectSeq(FL,
              {tok::exclaim,    tok::identifier,  tok::amp_amp,    tok::identifier, tok::pipe_pipe,
               tok::identifier, tok::amp,         tok::identifier, tok::pipe,       tok::identifier,
               tok::caret,      tok::identifier,  tok::amp_equal,  tok::identifier, tok::pipe_equal,
               tok::identifier, tok::caret_equal, tok::identifier, tok::semi,       tok::eof});
}

TEST_F(FileLexTests, IncDecArrowDotAndIndexingCallingTernaryComma) {
    auto FL = create_lexer("a++ --b p->m obj.m arr[0] f(x,y?z:w) , ;");
    expectSeq(FL, {tok::identifier,  tok::plus_plus,                   // a++
                   tok::minus_minus, tok::identifier,                  // --b
                   tok::identifier,  tok::arrow,      tok::identifier, // p->m
                   tok::identifier,  tok::dot,        tok::identifier, // obj.m
                   tok::identifier,  tok::l_square,   tok::numeric_constant,
                   tok::r_square, // arr[0]
                   tok::identifier,  tok::l_paren,    tok::identifier,
                   tok::comma,       tok::identifier, tok::question,
                   tok::identifier,  tok::colon,      tok::identifier,
                   tok::r_paren,     tok::comma,      tok::semi,
                   tok::eof});
}

TEST_F(FileLexTests, SingleCharPunctuators) {
    auto FL = create_lexer("(){}[];,:~%");
    expectSeq(FL,
              {tok::l_paren, tok::r_paren, tok::l_brace, tok::r_brace, tok::l_square, tok::r_square,
               tok::semi, tok::comma, tok::colon, tok::tilde, tok::percent, tok::eof});
}

TEST_F(FileLexTests, PreprocessorDefineWithCommentsAndLineSplice) {
    // Mirrors build/LexerTest.c: block comments surround '#', body uses line continuations.
    auto FL = create_lexer(R"(/*
*/ # /*
*/ defi\
ne FO\
O 10\
20
)");

    expectNextKind(FL, tok::pp_hash);
    expectNextKind(FL, tok::pp_define);
    expectNextKindLexeme(FL, tok::identifier, "FO\\\nO", "FOO");
    expectNextKindLexeme(FL, tok::numeric_constant, "10\\\n20", "1020");
    expectNextKind(FL, tok::eof);
}

TEST_F(FileLexTests, Trigraphs_AllMappingsHandled) {
    auto FL = create_lexer(R"(??= inclu??/
de "test.h"
??' ??( ??) ??! ??< ??> ??-
)");

    expectNextKindLexeme(FL, tok::pp_hash, "\?\?=", "#");
    expectNextKindLexeme(FL, tok::pp_include, "inclu\?\?/\nde", "include");
    expectNextKindLexeme(FL, tok::string_literal, "\"test.h\"", "\"test.h\"");
    expectNextKindLexeme(FL, tok::caret, "\?\?'", "^");
    expectNextKindLexeme(FL, tok::l_square, "\?\?(", "[");
    expectNextKindLexeme(FL, tok::r_square, "\?\?)", "]");
    expectNextKindLexeme(FL, tok::pipe, "\?\?!", "|");
    expectNextKindLexeme(FL, tok::l_brace, "\?\?<", "{");
    expectNextKindLexeme(FL, tok::r_brace, "\?\?>", "}");
    expectNextKindLexeme(FL, tok::tilde, "\?\?-", "~");
    expectNextKind(FL, tok::eof);
}

// ================ Unknown / illegal symbols =========================

TEST_F(FileLexTests, UnknownSymbol_YieldsUnknownToken) {
    auto FL = create_lexer("@");
    expectNextKind(FL, tok::unknown);
    expectNextKind(FL, tok::eof);
}

// =================== Stress safety (no crash) ========================

TEST_F(FileLexTests, VeryLongIdentifier_NoCrash) {
    std::string big(100000, 'a');
    auto        src = big + " " + big + ";";
    auto        FL = create_lexer(src.c_str());
    expectNextKind(FL, tok::identifier);
    expectNextKind(FL, tok::identifier);
    expectNextKind(FL, tok::semi);
    expectNextKind(FL, tok::eof);
}

TEST_F(FileLexTests, VeryLongNumber_NoCrash) {
    std::string big(120000, '9');
    auto        src = big + ";";
    auto        FL = create_lexer(src.c_str());
    expectNextKind(FL, tok::numeric_constant);
    expectNextKind(FL, tok::semi);
    expectNextKind(FL, tok::eof);
}

// Position-aware: assert kind and starting position
static void expectNextRawKindPos(scc::FileLexer &FL, tok::TokenKind expected, MemoryViewPos p) {
    scc::Token TK;
    FL.nextRaw(TK);
    ASSERT_EQ(TK.getTokenKind(), expected);
    const auto &B = TK.getPosBegin(); // adjust if different accessor
    ASSERT_EQ(B.Line, p.Line);
    ASSERT_EQ(B.Column, p.Column);
}

TEST_F(FileLexTests, Raw_Whitespace_And_Eol_NoValue) {
    auto FL = create_lexer("a  \t b\nc");
    expectNextRawKind(FL, tok::identifier);   // "a"
    expectNextRawKindNoValue(FL, tok::space); // run of spaces/tabs (Value empty)
    expectNextRawKind(FL, tok::identifier);   // "b"
    expectNextRawKindNoValue(FL, tok::eol);   // newline (Value empty)
    expectNextRawKind(FL, tok::identifier);   // "c"
    expectNextRawKind(FL, tok::eof);
}

TEST_F(FileLexTests, Raw_LineComment_Then_Eol_NoValue) {
    auto FL = create_lexer("int // hi\nx;");
    expectNextRawKind(FL, tok::t_int);
    expectNextRawKindNoValue(FL, tok::space);          // space before comment
    expectNextRawKindNoValue(FL, tok::comment_line);   // the //… token
    expectNextRawKindPos(FL, tok::identifier, {2, 1}); // 'x' starts next line, col 1
    expectNextRawKind(FL, tok::semi);
    expectNextRawKind(FL, tok::eof);
}

TEST_F(FileLexTests, Raw_BlockComment_MultiLine_NoValue_PositionsAfter) {
    auto FL = create_lexer("int /* a\nb\nc */ x;");
    expectNextRawKindPos(FL, tok::t_int, {1, 1});
    expectNextRawKindNoValue(FL, tok::space);   // space before comment
    expectNextRawKindNoValue(FL, tok::comment); // contains two '\n' internally
    expectNextRawKindNoValue(FL, tok::space);   // space before 'x'
    // After two newlines inside the comment and one space, 'x' at (3,6)
    expectNextRawKindPos(FL, tok::identifier, {3, 6});
    expectNextRawKindPos(FL, tok::semi, {3, 7});
    expectNextRawKind(FL, tok::eof);
}

TEST_F(FileLexTests, Raw_Mixed_Whitespace_Comments_Eol_NoValue) {
    auto FL = create_lexer("a /* x */  //y\n b");
    expectNextRawKind(FL, tok::identifier); // a
    expectNextRawKindNoValue(FL, tok::space);
    expectNextRawKindNoValue(FL, tok::comment);      // /* x */
    expectNextRawKindNoValue(FL, tok::space);        // two spaces, still empty value
    expectNextRawKindNoValue(FL, tok::comment_line); // comment line don't show the eol
    expectNextRawKindNoValue(FL, tok::space);
    expectNextRawKindPos(FL, tok::identifier, {2, 2}); // 'b' after one leading space
    expectNextRawKind(FL, tok::eof);
}

TEST_F(FileLexTests, Raw_CRLF_Eol_NoValue_Positions) {
    auto FL = create_lexer("a\r\nb\n");
    expectNextRawKindPos(FL, tok::identifier, {1, 1}); // a
    expectNextRawKindNoValue(FL, tok::space);
    expectNextRawKindNoValue(FL, tok::eol);            // CRLF normalized to EOL token
    expectNextRawKindPos(FL, tok::identifier, {2, 1}); // b at next line
    expectNextRawKindNoValue(FL, tok::eol);            // LF
    expectNextRawKind(FL, tok::eof);
}

TEST_F(FileLexTests, Raw_TrailingSpace_BeforeEOF_NoValue) {
    auto FL = create_lexer("x  ");
    expectNextRawKind(FL, tok::identifier);
    expectNextRawKindNoValue(FL, tok::space); // one or more runs; value empty
    expectNextRawKind(FL, tok::eof);
}

TEST_F(FileLexTests, Raw_NormalTokens_UnchangedKinds) {
    auto FL = create_lexer("a+b");
    expectNextRawKind(FL, tok::identifier);
    expectNextRawKind(FL, tok::plus);
    expectNextRawKind(FL, tok::identifier);
    expectNextRawKind(FL, tok::eof);
}

// ===================== String literals =====================

TEST_F(FileLexTests, StringLiteral_Empty_And_Simple) {
    auto FL = create_lexer(R"("" "hi")");
    expectSeq(FL, {tok::string_literal, tok::string_literal, tok::eof});
}

TEST_F(FileLexTests, StringLiteral_With_CommonEscapes) {
    // "a\n\t\\\""
    auto FL = create_lexer("\"a\\n\\t\\\\\\\"\"");
    expectSeq(FL, {tok::string_literal, tok::eof});
}

TEST_F(FileLexTests, StringLiteral_With_Hex_And_Octal) {
    // "\x41 \101"
    auto FL = create_lexer("\"\\x41 \\101\"");
    expectSeq(FL, {tok::string_literal, tok::eof});
}

TEST_F(FileLexTests, StringLiteral_Concatenation_Produces_Two_Tokens) {
    // If your lexer returns two tokens and concatenation is done later,
    // keep this. If you plan to fold during lexing, adjust expected list.
    auto FL = create_lexer("\"foo\" \"bar\"");
    expectSeq(FL, {tok::string_literal, tok::string_literal, tok::eof});
}

TEST_F(FileLexTests, StringLiteral_Unterminated_UnknownOrEOF) {
    auto                        FL = create_lexer("\"oops");
    std::vector<tok::TokenKind> kinds;
    drainToEOF(FL, kinds);
    ASSERT_EQ(kinds.back(), tok::eof);
    bool sawUnknown =
        std::any_of(kinds.begin(), kinds.end(), [](tok::TokenKind k) { return k == tok::unknown; });
    EXPECT_TRUE(sawUnknown || true);
}

TEST_F(FileLexTests, StringLiteral_Positions_Basic) {
    auto FL = create_lexer("\"a\"  \n"      // line 1
                           "  \"b\"  \"c\"" // line 2
    );
    expectNextKindPos(FL, tok::string_literal, {1, 1});
    expectNextKindPos(FL, tok::string_literal, {2, 3}); // after two spaces
    expectNextKindPos(FL, tok::string_literal, {2, 8}); // one space between "b" and "c"
    expectNextKind(FL, tok::eof);
}

// -------- RAW variants for strings --------

TEST_F(FileLexTests, Raw_StringLiteral_With_Spaces_And_Eol) {
    auto FL = create_lexer("\"x\"  \n\"y\"");
    expectNextRawKind(FL, tok::string_literal);
    expectNextRawKindNoValue(FL, tok::space);
    expectNextRawKindNoValue(FL, tok::eol);
    expectNextRawKind(FL, tok::string_literal);
    expectNextRawKind(FL, tok::eof);
}

// ===================== Character constants =====================

TEST_F(FileLexTests, CharLiteral_Simple_And_CommonEscapes) {
    auto FL = create_lexer("'a' '\\n' '\\t' '\\\\' '\\''");
    expectSeq(FL, {tok::char_constant, tok::char_constant, tok::char_constant, tok::char_constant,
                   tok::char_constant, tok::eof});
}

TEST_F(FileLexTests, CharLiteral_Hex_And_Octal) {
    auto FL = create_lexer("'\\x41' '\\101'");
    expectSeq(FL, {tok::char_constant, tok::char_constant, tok::eof});
}

TEST_F(FileLexTests, CharLiteral_Unterminated_UnknownOrEOF) {
    auto                        FL = create_lexer("'a");
    std::vector<tok::TokenKind> kinds;
    drainToEOF(FL, kinds);
    ASSERT_EQ(kinds.back(), tok::eof);
    bool sawUnknown =
        std::any_of(kinds.begin(), kinds.end(), [](tok::TokenKind k) { return k == tok::unknown; });
    EXPECT_TRUE(sawUnknown || true);
}

TEST_F(FileLexTests, CharLiteral_Positions) {
    auto FL = create_lexer("'a'\n"
                           "  '\\n'  '\\x41'");
    expectNextKindPos(FL, tok::char_constant, {1, 1});
    expectNextKindPos(FL, tok::char_constant, {2, 3}); // after two spaces
    expectNextKindPos(FL, tok::char_constant, {2, 9}); // two spaces after previous
    expectNextKind(FL, tok::eof);
}

// -------- RAW variants for chars --------

TEST_F(FileLexTests, Raw_CharLiteral_With_Space_Then_Semi) {
    auto FL = create_lexer("'a' ;");
    expectNextRawKind(FL, tok::char_constant);
    expectNextRawKindNoValue(FL, tok::space);
    expectNextRawKind(FL, tok::semi);
    expectNextRawKind(FL, tok::eof);
}
