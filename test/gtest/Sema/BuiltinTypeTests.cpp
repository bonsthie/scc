#include <gtest/gtest.h>

#include "scc/AST/ASTContext.h"
#include "scc/AST/BuiltinType.h"
#include "scc/Error/ErrorManager.h"
#include "scc/Frontend/LangOpt.h"
#include "scc/Sema/Sema.h"
#include "scc/Token/Token.h"

using namespace scc;

TEST(SemaBuiltinTypeTest, ReturnsBuiltinTypeForTypeKeywordToken) {
    ASTContext   Ctx;
    ErrorManager EM;
    LangOpt      Opts;
    Sema         S(Ctx, EM, Opts);
    Token        Tok(tok::t_int);

    const Type *Ty = S.getType(Tok);

    ASSERT_NE(nullptr, Ty);
    ASSERT_TRUE(Ty->isBuiltinType());
    EXPECT_EQ(TYint, static_cast<const BuiltinType *>(Ty)->getBuiltinKind());
}

TEST(SemaBuiltinTypeTest, ReturnsUnknownTypeForUnknownIdentifier) {
    ASTContext   Ctx;
    ErrorManager EM;
    LangOpt      Opts;
    Sema         S(Ctx, EM, Opts);
    Token        Tok(tok::identifier);

    const Type *Ty = S.getType(Tok);

    ASSERT_NE(nullptr, Ty);
    EXPECT_TRUE(Ty->isUnknow());
}

TEST(SemaBuiltinTypeTest, ReturnsNullForUnknownIdentifierInTypeSpecifierContext) {
    ASTContext   Ctx;
    ErrorManager EM;
    LangOpt      Opts;
    Sema         S(Ctx, EM, Opts);
    Token        Tok(tok::identifier);

    EXPECT_EQ(nullptr, S.getTypeSpecifierType(Tok));
}

TEST(SemaBuiltinTypeTest, ReturnsSharedCanonicalBuiltinQualType) {
    ASTContext   Ctx;
    ErrorManager EM;
    LangOpt      Opts;
    Sema         S(Ctx, EM, Opts);

    const CanQualType *QT = S.getBuiltinType(TYint);

    ASSERT_NE(nullptr, QT);
    ASSERT_FALSE(QT->isNull());
    ASSERT_NE(nullptr, QT->asQualType().getType());
    ASSERT_TRUE(QT->asQualType().getType()->isBuiltinType());
    EXPECT_EQ(TYint, static_cast<const BuiltinType *>(QT->asQualType().getType())->getBuiltinKind());
}
