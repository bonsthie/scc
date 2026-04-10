#include "gtest/gtest.h"

#include <sstream>

#include "scc/AST/BuiltinType.h"
#include "scc/Parser/ParsedDeclSpec.h"

using namespace scc;

TEST(ParsedDeclSpecTest, DefaultsToNoTypeSpecifier) {
    ParsedDeclSpec DS;

    EXPECT_EQ(nullptr, DS.T);
    EXPECT_FALSE(DS.hasTypeSpecifier());
    EXPECT_EQ(SignSpecifier::Unspecified, DS.getSignSpecifier());
    EXPECT_EQ(tok::not_init, DS.getSignSpecifierTokenKind());
    EXPECT_EQ(LengthSpecifier::Unspecified, DS.getLengthSpecifier());
    EXPECT_EQ(tok::not_init, DS.getLengthSpecifierTokenKind());
}

TEST(ParsedDeclSpecTest, AddingTypeSpecifierMarksTypeSpecifierPresent) {
    ParsedDeclSpec DS;
    BuiltinType    IntTy(TYint);
    Token          Tok(tok::t_int);

    EXPECT_TRUE(DS.tryAddTypeSpecifier(&IntTy, Tok.getRange()));
    EXPECT_TRUE(DS.hasTypeSpecifier());
    EXPECT_EQ(&IntTy, DS.T);
    EXPECT_EQ(Tok.getRange().FID, DS.TypeSourceRange.FID);
    EXPECT_EQ(Tok.getRange().Begin.Line, DS.TypeSourceRange.Begin.Line);
    EXPECT_EQ(Tok.getRange().Begin.Column, DS.TypeSourceRange.Begin.Column);
    EXPECT_EQ(Tok.getRange().End.Line, DS.TypeSourceRange.End.Line);
    EXPECT_EQ(Tok.getRange().End.Column, DS.TypeSourceRange.End.Column);
}

TEST(ParsedDeclSpecTest, RejectsSecondTypeSpecifierAfterTypeSpecifier) {
    ParsedDeclSpec DS;
    SourceRange    Range;
    BuiltinType    IntTy(TYint);
    BuiltinType    FloatTy(TYfloat);

    ASSERT_TRUE(DS.tryAddTypeSpecifier(&IntTy, Range));
    EXPECT_FALSE(DS.tryAddTypeSpecifier(&FloatTy, Range));
}

TEST(ParsedDeclSpecTest, RejectsNullTypeSpecifier) {
    ParsedDeclSpec DS;
    SourceRange    Range;

    EXPECT_FALSE(DS.tryAddTypeSpecifier(nullptr, Range));
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
    BuiltinType    IntTy(TYint);
    SourceRange    Range;
    std::ostringstream Out;

    ASSERT_TRUE(DS.tryAddStorageSpecifier(StorageClassSpecifier::Static));
    ASSERT_TRUE(DS.tryAddSignSpecifier(SignSpecifier::Unsigned, Range));
    ASSERT_TRUE(DS.tryAddLengthSpecifier(LengthSpecifier::Long, Range));
    ASSERT_TRUE(DS.tryAddLengthSpecifier(LengthSpecifier::Long, Range));
    ASSERT_TRUE(DS.tryAddConst());
    ASSERT_TRUE(DS.tryAddRestrict());
    ASSERT_TRUE(DS.tryAddVolatile());
    ASSERT_TRUE(DS.tryAddTypeSpecifier(&IntTy, Range));

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
