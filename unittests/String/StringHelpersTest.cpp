#include <gtest/gtest.h>

#include "scc/String/StringInterner.h"
#include "scc/String/StringSwitch.h"
#include "scc/String/StringUtils.h"

#include <string>
#include <string_view>

using namespace scc;

TEST(StringInternerTest, DeduplicatesStringsAndKeepsStorageStable) {
    StringInterner SI;

    std::string_view Hello = SI.intern("hello");
    std::string_view HelloAgain = SI.intern(std::string("hello"));
    std::string_view World = SI.intern("world");

    EXPECT_EQ(Hello.data(), HelloAgain.data());
    EXPECT_NE(Hello.data(), World.data());

    // Empty strings stay empty.
    EXPECT_TRUE(SI.intern("").empty());
}

TEST(StringUtilsTest, TrimsQuotedStrings) {
    std::string Quote = "\"value\"";
    EXPECT_EQ(trim_quote(Quote), "value");

    std::string NotQuoted = "plain";
    EXPECT_EQ(trim_quote(NotQuoted), "plain");

    std::string Angle = "<system>";
    EXPECT_EQ(trim_quote_system(Angle), "system");
}

TEST(StringSwitchTest, MatchesCasesOrFallsBackToDefault) {
    StringSwitch<int> SS("beta");
    int               Value = SS.Case("alpha", 1).Case("beta", 2).Default(3);
    EXPECT_EQ(Value, 2);

    StringSwitch<int> DefaultOnly("zeta");
    EXPECT_EQ(DefaultOnly.Default(42), 42);
}
