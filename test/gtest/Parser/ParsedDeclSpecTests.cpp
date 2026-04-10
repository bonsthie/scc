#include "gtest/gtest.h"

#include <sstream>

#include "scc/AST/BuiltinType.h"
#include "scc/Parser/ParsedDeclSpec.h"

using namespace scc;

TEST(ParsedDeclSpecTest, DefaultsToNoTypeSpecifier) {
    ParsedDeclSpec DS;

    EXPECT_EQ(nullptr, DS.T);
    EXPECT_EQ(TYunspecified, DS.BuiltinTy);
    EXPECT_FALSE(DS.hasTypeSpecifier());
    EXPECT_EQ(SignSpecifier::Unspecified, DS.getSignSpecifier());
    EXPECT_EQ(tok::not_init, DS.getSignSpecifierTokenKind());
    EXPECT_EQ(LengthSpecifier::Unspecified, DS.getLengthSpecifier());
    EXPECT_EQ(tok::not_init, DS.getLengthSpecifierTokenKind());
    EXPECT_EQ(TYunspecified, DS.getBuiltinTypeSpecifier());
}

TEST(ParsedDeclSpecTest, AddingBuiltinTypeMarksTypeSpecifierPresent) {
    ParsedDeclSpec DS;
    Token          Tok(tok::t_int);

    EXPECT_TRUE(DS.tryAddBuiltinTypeSpecifier(Tok));
    EXPECT_TRUE(DS.hasTypeSpecifier());
    EXPECT_EQ(nullptr, DS.T);
    EXPECT_EQ(TYint, DS.BuiltinTy);
    EXPECT_EQ(TYint, DS.getBuiltinTypeSpecifier());
    EXPECT_EQ(Tok.getRange().FID, DS.getBuiltinTypeSpecifierRange().FID);
    EXPECT_EQ(Tok.getRange().Begin.Line, DS.getBuiltinTypeSpecifierRange().Begin.Line);
    EXPECT_EQ(Tok.getRange().Begin.Column, DS.getBuiltinTypeSpecifierRange().Begin.Column);
    EXPECT_EQ(Tok.getRange().End.Line, DS.getBuiltinTypeSpecifierRange().End.Line);
    EXPECT_EQ(Tok.getRange().End.Column, DS.getBuiltinTypeSpecifierRange().End.Column);
}

TEST(ParsedDeclSpecTest, RejectsSecondTypeSpecifierAfterBuiltinType) {
    ParsedDeclSpec DS;
    SourceRange    Range;
    BuiltinType    FloatTy(TYfloat);
    Token          Tok(tok::t_int);

    ASSERT_TRUE(DS.tryAddBuiltinTypeSpecifier(Tok));
    EXPECT_FALSE(DS.tryAddTypeSpecifier(&FloatTy, Range));
}

TEST(ParsedDeclSpecTest, RejectsUnspecifiedBuiltinType) {
    ParsedDeclSpec DS;
    Token          Tok;

    EXPECT_FALSE(DS.tryAddBuiltinTypeSpecifier(Tok));
    EXPECT_FALSE(DS.hasTypeSpecifier());
}

TEST(ParsedDeclSpecTest, TracksSignAndLengthSpecifierTokensAndRanges) {
    ParsedDeclSpec DS;
    Token          SignedTok(tok::t_signed);
    Token          LongTok(tok::t_long);

    SignedTok.setRange(SourceRange(nullptr, MemoryViewPos(2, 3), MemoryViewPos(2, 8)));
    LongTok.setRange(SourceRange(nullptr, MemoryViewPos(4, 5), MemoryViewPos(4, 8)));

    ASSERT_TRUE(DS.tryAddSignSpecifier(SignSpecifier::Signed, SignedTok.getRange()));
    ASSERT_TRUE(DS.tryAddLengthSpecifier(LengthSpecifier::Long, LongTok.getRange()));

    EXPECT_EQ(SignSpecifier::Signed, DS.getSignSpecifier());
    EXPECT_EQ(tok::t_signed, DS.getSignSpecifierTokenKind());
    EXPECT_EQ(SignedTok.getRange().Begin.Line, DS.getSignSpecifierRange().Begin.Line);
    EXPECT_EQ(SignedTok.getRange().Begin.Column, DS.getSignSpecifierRange().Begin.Column);

    EXPECT_EQ(LengthSpecifier::Long, DS.getLengthSpecifier());
    EXPECT_EQ(tok::t_long, DS.getLengthSpecifierTokenKind());
    EXPECT_EQ(LongTok.getRange().Begin.Line, DS.getLengthSpecifierRange().Begin.Line);
    EXPECT_EQ(LongTok.getRange().Begin.Column, DS.getLengthSpecifierRange().Begin.Column);
}

TEST(ParsedDeclSpecTest, LengthSpecifierOnlyFormsLongLongFromTwoLongs) {
    ParsedDeclSpec DS;
    SourceRange    Range;

    ASSERT_TRUE(DS.tryAddLengthSpecifier(LengthSpecifier::Long, Range));
    ASSERT_TRUE(DS.tryAddLengthSpecifier(LengthSpecifier::Long, Range));

    EXPECT_EQ(LengthSpecifier::LongLong, DS.getLengthSpecifier());
}

TEST(ParsedDeclSpecTest, LengthSpecifierRejectsInvalidSecondLength) {
    ParsedDeclSpec ShortThenShort;
    ParsedDeclSpec ShortThenLong;
    ParsedDeclSpec LongThenShort;
    SourceRange    Range;

    ASSERT_TRUE(ShortThenShort.tryAddLengthSpecifier(LengthSpecifier::Short, Range));
    EXPECT_FALSE(ShortThenShort.tryAddLengthSpecifier(LengthSpecifier::Short, Range));

    ASSERT_TRUE(ShortThenLong.tryAddLengthSpecifier(LengthSpecifier::Short, Range));
    EXPECT_FALSE(ShortThenLong.tryAddLengthSpecifier(LengthSpecifier::Long, Range));

    ASSERT_TRUE(LongThenShort.tryAddLengthSpecifier(LengthSpecifier::Long, Range));
    EXPECT_FALSE(LongThenShort.tryAddLengthSpecifier(LengthSpecifier::Short, Range));
}

TEST(ParsedDeclSpecTest, PrintUsesRequestedSpecifierOrderWithBuiltinType) {
    ParsedDeclSpec DS;
    Token          IntTok(tok::t_int);
    SourceRange    Range;
    std::ostringstream Out;

    ASSERT_TRUE(DS.tryAddStorageSpecifier(StorageClassSpecifier::Static));
    ASSERT_TRUE(DS.tryAddSignSpecifier(SignSpecifier::Unsigned, Range));
    ASSERT_TRUE(DS.tryAddLengthSpecifier(LengthSpecifier::Long, Range));
    ASSERT_TRUE(DS.tryAddLengthSpecifier(LengthSpecifier::Long, Range));
    ASSERT_TRUE(DS.tryAddConst());
    ASSERT_TRUE(DS.tryAddRestrict());
    ASSERT_TRUE(DS.tryAddVolatile());
    ASSERT_TRUE(DS.tryAddBuiltinTypeSpecifier(IntTok));

    DS.print(Out);
    EXPECT_EQ("static unsigned long long const restrict volatile int", Out.str());
}

TEST(ParsedDeclSpecTest, PrintUsesTypeWhenBuiltinTypeSpecifierIsAbsent) {
    ParsedDeclSpec   DS;
    BuiltinType      FloatTy(TYfloat);
    SourceRange      Range;
    std::ostringstream Out;

    ASSERT_TRUE(DS.tryAddTypeSpecifier(&FloatTy, Range));

    DS.print(Out);
    EXPECT_EQ("float", Out.str());
}
