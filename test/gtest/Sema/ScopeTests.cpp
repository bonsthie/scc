#include <gtest/gtest.h>

#include "scc/AST/Decl.h"
#include "scc/Sema/Scope.h"

using namespace scc;

namespace {

struct DummyDecl : Decl {
    DummyDecl() : Decl(DeclKind::None) {}
};

} // namespace

TEST(ScopeTest, RejectsDuplicateSymbols) {
    DummyDecl First, Second;
    Scope     S;

    EXPECT_TRUE(S.addDecl("foo", &First));
    EXPECT_FALSE(S.addDecl("foo", &Second));
    EXPECT_EQ(S.lookup("foo"), &First);
    EXPECT_EQ(S.lookup("bar"), nullptr);
}

TEST(ScopeMgrTest, PrefersInnermostScope) {
    DummyDecl Outer, Inner;
    ScopeMgr  SM;

    SM.newScope();
    ASSERT_TRUE(SM.addDecl("value", &Outer));

    SM.newScope();
    ASSERT_TRUE(SM.addDecl("value", &Inner));

    EXPECT_EQ(SM.lookup("value"), &Inner);

    SM.popScope();
    EXPECT_EQ(SM.lookup("value"), &Outer);
}
