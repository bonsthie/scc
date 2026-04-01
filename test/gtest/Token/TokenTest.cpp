#include <gtest/gtest.h>

#include "scc/Token/Token.h"

#include <sstream>
#include <string>

using namespace scc;

TEST(TokenTest, TracksRangeAndDirtyState) {
    FileID        FID("/tmp/file.c", 1);
    Token         T(tok::identifier, "foo");
    MemoryViewPos Begin{1, 1};
    MemoryViewPos End{1, 4};

    T.setFileID(&FID);
    T.setPosBegin(Begin);
    T.setPosEnd(End);

    EXPECT_EQ(T.getRange().FID, &FID);
    EXPECT_EQ(T.posViewBegin().P.Column, 1u);
    EXPECT_EQ(T.posViewEnd().P.Column, 4u);

    T.setDirtyValue("raw");
    T.setDirty(true);
    EXPECT_TRUE(T.isDirty());

    T.flush();
    EXPECT_TRUE(T.is(tok::not_init));
    EXPECT_FALSE(T.isDirty());
}

TEST(TokenTest, VariadicIsHelperAndKeywordCreation) {
    Token T(tok::identifier, "int");

    EXPECT_TRUE(T.is(tok::identifier, tok::numeric_constant));

    create_keyword_token(T);
    EXPECT_EQ(T.getTokenKind(), tok::t_int);
}

TEST(TokenTest, CleanTokenRemovesEscapes) {
    std::string raw = "line\\\ncontinuation";
    std::string clean = clean_token(raw);

    EXPECT_EQ(clean, "linecontinuation");
}

TEST(TokenTest, StringifyTokenKindProducesNames) {
    EXPECT_EQ(stringify_token_kind(tok::plus_equal), "plus_equal");
}
