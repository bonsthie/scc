#include <gtest/gtest.h>

#include "scc/AST/ASTContext.h"
#include "scc/AST/BuiltinType.h"
#include "scc/Frontend/FrontendErrorManager.h"
#include "scc/Frontend/LangOpt.h"
#include "scc/Sema/Sema.h"
#include "scc/Token/Token.h"

using namespace scc;

TEST(SemaBuiltinTypeTest, ReturnsBuiltinTypeForTypeKeywordToken) {
    ASTContext   Ctx;
    FrontendErrorManager EM;
    LangOpt      Opts;
    Sema         S(Ctx, EM, Opts);
    Token        Tok(tok::t_int);

    const Type *Ty = S.getType(Tok);

    ASSERT_NE(nullptr, Ty);
    ASSERT_TRUE(Ty->isBuiltinType());
    EXPECT_EQ(TYint, static_cast<const BuiltinType *>(Ty)->getBuiltinKind());
}

TEST(SemaBuiltinTypeTest, ReturnsBuiltinTypeForBuiltinTypeKind) {
    ASTContext   Ctx;
    FrontendErrorManager EM;
    LangOpt      Opts;
    Sema         S(Ctx, EM, Opts);

    const Type *Ty = S.getType(TYint);

    ASSERT_NE(nullptr, Ty);
    ASSERT_TRUE(Ty->isBuiltinType());
    EXPECT_EQ(TYint, static_cast<const BuiltinType *>(Ty)->getBuiltinKind());
}

TEST(SemaBuiltinTypeTest, ReturnsUnknownTypeForUnknownIdentifier) {
    ASTContext   Ctx;
    FrontendErrorManager EM;
    LangOpt      Opts;
    Sema         S(Ctx, EM, Opts);
    Token        Tok(tok::identifier);

    const Type *Ty = S.getType(Tok);

    ASSERT_NE(nullptr, Ty);
    EXPECT_TRUE(Ty->isUnknow());
}

TEST(SemaBuiltinTypeTest, ReturnsUnknownTypeForUnknownIdentifierInTypeSpecifierContext) {
    ASTContext   Ctx;
    FrontendErrorManager EM;
    LangOpt      Opts;
    Sema         S(Ctx, EM, Opts);
    Token        Tok(tok::identifier);

    const Type *Ty = S.getTypeSpecifierType(Tok);

    ASSERT_NE(nullptr, Ty);
    EXPECT_TRUE(Ty->isUnknow());
}

TEST(SemaBuiltinTypeTest, ReturnsSharedCanonicalBuiltinQualType) {
    ASTContext   Ctx;
    FrontendErrorManager EM;
    LangOpt      Opts;
    Sema         S(Ctx, EM, Opts);

    const CanQualType *QT = S.getBuiltinType(TYint);

    ASSERT_NE(nullptr, QT);
    ASSERT_FALSE(QT->isNull());
    ASSERT_NE(nullptr, QT->asQualType().getType());
    ASSERT_TRUE(QT->asQualType().getType()->isBuiltinType());
    EXPECT_EQ(TYint, static_cast<const BuiltinType *>(QT->asQualType().getType())->getBuiltinKind());
}

TEST(SemaDeclSpecTest, ReportsUnknownTypeNameForUnknownTypeSpecifier) {
    ASTContext     Ctx;
    FrontendErrorManager EM;
    LangOpt        Opts;
    Sema           S(Ctx, EM, Opts);
    ParsedDeclSpec DS;
    Type           UnknownTy(TypeKind::Unknow);

    DS.T = &UnknownTy;
    DS.TypeName = "foo";
    DS.TypeSourceRange = SourceRange(nullptr, {2, 9}, {2, 12});

    EXPECT_TRUE(S.actOnDeclSpec(DS));
    ASSERT_EQ(1, EM.size());
    EXPECT_EQ("unknown type name 'foo'", EM.getErrsList()[0]->getMsg());
}

TEST(SemaDeclSpecTest, ReportsMissingTypeSpecifierForImplicitInt) {
    ASTContext     Ctx;
    FrontendErrorManager EM;
    LangOpt        Opts;
    ParsedDeclSpec DS;
    Opts.implicit_intWarning = true;
    Sema           S(Ctx, EM, Opts);

    DS.TypeName = "bar";
    DS.TypeSourceRange = SourceRange(nullptr, {3, 9}, {3, 12});

    EXPECT_TRUE(S.actOnDeclSpec(DS));
    ASSERT_EQ(1, EM.size());
    EXPECT_EQ("type specifier missing, defaults to 'int'; ISO C99 and later do not support "
              "implicit int [-Wimplicit-int]",
              EM.getErrsList()[0]->getMsg());
}
