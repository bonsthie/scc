Happy-path structure

EmptyInput_YieldsEOF
"" → [eof]

HelloWorld_Main
"int main() { return 0; }" → [t_int, identifier, l_paren, r_paren, l_brace, kw_return, numeric_constant, semi, r_brace, eof]
(your existing BasicTest)

SimpleParams_CommaSeparated
"int f(int a, float b) {}" → [t_int, identifier, l_paren, t_int, identifier, comma, t_float, identifier, r_paren, l_brace, r_brace, eof]

2) Identifiers & keywords

Identifier_Shapes
"_a a1 __x9" → [identifier, identifier, identifier, eof]

Keyword_Int_And_Return
"int x; return;" → [t_int, identifier, semi, kw_return, semi, eof]

All_C_Keywords_TokenizeAsKeywords
One string containing all keywords you support (int, char, float, double, void, if, else, for, while, do, break, continue, return, switch, case, default, struct, union, enum, typedef, const, volatile, static, extern, inline, sizeof, signed, unsigned, long, short, auto, register, … plus any C11 you implement).
Expect each as kw_* or dedicated t_*. (Parameterize if you like.)

IdentifierVsKeyword_Prefix
"integer returnx" → [identifier, identifier, eof] (ensure keywords don’t match as prefixes)

3) Literals

Integer_Decimal
"0 7 42 123456" → four [numeric_constant] then eof.

Integer_Hex_Oct_Bin
"0xFF 0xff 077 0b1011" → four [numeric_constant], eof.

Integer_Suffixes
"1u 2U 3l 4L 5ul 6ULL" → each [numeric_constant] with preserved lexemes.

Float_Basics
"0. .5 1. 1.0 1e9 1.5e-2 3.E+7" → all [numeric_constant].

Float_Suffixes
"1.0f 2.F 3e10L" → [numeric_constant]*3.

CharLiteral_SimpleAndEscapes
"'a' '\\n' '\\'' '\\x41'" → [char_constant]*4.

StringLiteral_Simple
"\"hi\"" → [string_literal].

StringLiteral_Escapes_AndHex
"\"a\\n\\t\\x41\"" → [string_literal].

StringLiteral_Concatenation
"\"foo\" \"bar\"" → two [string_literal].

Wide_U8_U16_U32_StringPrefixes (if supported)
u8"…" u"…" U"…" L"…" → [string_literal]*4.

4) Operators & punctuators

SingleChar_Punctuators
"(){}[];,.:?~" → the obvious kinds.

Assignment_AndEquals
"= += -= *= /= %=" → [assign, plus_equal, minus_equal, star_equal, slash_equal, percent_equal].

Bitwise_AndEquals
"& &= | |= ^ ^=" → [amp, amp_equal, pipe, pipe_equal, caret, caret_equal].

Logic_AndOrNot
"&& || ! == !=" → [ampamp, pipepipe, bang, eq, neq] (use your names).

Relational_AndShift
"< <= > >= << >> <<= >>= → [less, le, greater, ge, lshift, rshift, lshift_equal, rshift_equal].

IncDec_Arrow_Dot
"++ -- -> ." → [plusplus, minusminus, arrow, dot].

Slash_Star_AndCommentsDisambiguation
"* *= / /=" → [star, star_equal, slash, slash_equal].

5) Comments & whitespace

LineComment_SkipsToEOL
"int // cmt\n x;" → [t_int, identifier, semi].

BlockComment_Skips
"int /* hi */ x;" → [t_int, identifier, semi].

BlockComment_MultiLine
"int /* a\nb\nc */ x;" → [t_int, identifier, semi] and correct line numbers after.

NestedBlockComment_IfUnsupported_TreatAsEndAtFirstClose
"/* a /* b */ c */" → ensure your intended behavior (either error token or skip once).

Unterminated_BlockComment_EmitsErrorOrEOF
"/* oops" → expect your lexer’s error token (or just reach eof with an error flag).

Whitespace_Newlines_Tabs
"a \t \n b" → [identifier, identifier] with proper positions.

StartOfLine_Flag_IsSet
Input with multiple lines; assert the token at column 1 has your [StartOfLine] behavior (you mentioned this in your print()).

6) Preprocessor-ish (if your lexer handles them)

HashAtBol_ProducesHashToken
"#include <x>" → [hash, identifier, less, identifier, greater] or whatever your policy is.

PP_Identifiers_NotKeywords
"#define int x" → int must lex as identifier in PP mode if you have modes; otherwise skip this.

7) Numbers & dots corner cases

DotVsFloat_LeadingDot
". .5 .. ... .a" → tokens should be [dot, numeric_constant, dot, dot, dot, identifier] (typical C splits .. as two dots; adjust to your rules).

HexFloat_IfSupported
"0x1.fp+3" → [numeric_constant] (if you implemented hex floats).

DigitSeparator_IfSupported
"1'000 0xDEAD'BEEF" → numeric constants (if you support C++14 style; otherwise ensure they error).

8) Strings & chars edge cases

Unterminated_String_EmitsErrorOrEOF
"\"oops" → error token (or eof with error flag).

Invalid_Escape_Sequence
"\"\\q\"" → either accept raw or emit error token depending on your policy.

CharLiteral_EmptyOrMulti
"'' 'ab'" → error tokens.

9) Locations & file info

TokenLocations_LineCol
Multi-line input; assert a few tokens’ PosBegin.Line/Column are as expected.

FID_Name_Propagates
Ensure TK.FID->getName() matches "test File" and your print() only shows basename (add path in FID; expect basename).

10) Error & unknowns

UnknownSymbol_TurnsIntoErrorToken
"@" (or any unsupported) → [error].

VeryLongIdentifier_BufferSafety
Identifier of, say, 100k ‘a’s → single [identifier] and no crash.

VeryLongNumber_BufferSafety
Decimal with 100k digits → single [numeric_constant] or error; no crash.

11) Stress / sequences

ManyTokens_NoAllocThrash
A big sequence like "int a0; int a1; … int a999;" → cycles of [t_int, identifier, semi].

MixedEverything_Smoke
Toss in literals, ops, comments, strings across several lines; assert the high-level sequence (not every token) and that eof is reached without errors.
