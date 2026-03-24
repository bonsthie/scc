#include "gtest/gtest.h"

#include "scc/AST/BuiltinType.h"
#include "scc/AST/CanQualType.h"
#include "scc/AST/QualType.h"
#include "scc/AST/Type.h"

using namespace scc;

TEST(TypeTest, BuiltinTypeKindAndQuery) {
    BuiltinType IntTy(TYint);

    EXPECT_EQ(TypeKind::Builtin, IntTy.kind());
    EXPECT_TRUE(IntTy.isBuiltinType());
    EXPECT_FALSE(IntTy.isPointerType());
    EXPECT_EQ(TYint, IntTy.getBuiltinKind());
}

TEST(TypeTest, TypedefUnderlyingTypePreservesPointerAndQualifiers) {
    BuiltinType IntTy(TYint);
    Qualifiers  UnderQuals{};
    UnderQuals.IsConst = true;

    TypedefType Alias("MyInt", &IntTy, UnderQuals);
    QualType    Under = Alias.getUnderlyingType();

    EXPECT_EQ(&IntTy, Under.getType());
    EXPECT_TRUE(Under.isConstQualified());
    EXPECT_FALSE(Under.isVolatileQualified());
    EXPECT_FALSE(Under.isRestrictQualified());
}

TEST(QualTypeTest, DesugarOnceMergesQualifiers) {
    BuiltinType IntTy(TYint);

    Qualifiers UnderQuals{};
    UnderQuals.IsVolatile = true;
    TypedefType Alias("VolatileInt", &IntTy, UnderQuals);

    Qualifiers AliasQuals{};
    AliasQuals.IsConst = true;
    QualType QualifiedAlias(&Alias, AliasQuals);

    QualType Result = QualifiedAlias.desugarOnce();
    EXPECT_EQ(&IntTy, Result.getType());
    EXPECT_TRUE(Result.isConstQualified());
    EXPECT_TRUE(Result.isVolatileQualified());
    EXPECT_FALSE(Result.isRestrictQualified());
}

TEST(QualTypeTest, EqualityDependsOnTypeAndQualifiers) {
    BuiltinType IntTy(TYint);
    BuiltinType FloatTy(TYfloat);

    QualType Q1(&IntTy);
    QualType Q2(&IntTy);
    EXPECT_EQ(Q1, Q2);

    Qualifiers ConstQuals{};
    ConstQuals.IsConst = true;
    QualType ConstInt(&IntTy, ConstQuals);
    EXPECT_NE(Q1, ConstInt);

    QualType Float(&FloatTy);
    EXPECT_NE(Q1, Float);
}

TEST(CanQualTypeTest, CreateStripsTypedefChainAndMergesQualifiers) {
    BuiltinType IntTy(TYint);

    Qualifiers InnerQuals{};
    InnerQuals.IsVolatile = true;
    TypedefType Inner("Inner", &IntTy, InnerQuals);

    Qualifiers MiddleQuals{};
    MiddleQuals.IsConst = true;
    TypedefType Middle("Middle", &Inner, MiddleQuals);

    Qualifiers AliasQuals{};
    AliasQuals.IsRestrict = true;
    QualType Alias(&Middle, AliasQuals);

    CanQualType     Canon = CanQualType::create(Alias);
    const QualType &Result = Canon.asQualType();

    EXPECT_EQ(&IntTy, Result.getType());
    EXPECT_TRUE(Result.isConstQualified());
    EXPECT_TRUE(Result.isRestrictQualified());
    EXPECT_TRUE(Result.isVolatileQualified());
    EXPECT_FALSE(Result.isNull());
}

TEST(CanQualTypeTest, EqualityUsesCanonicalQualType) {
    BuiltinType IntTy(TYint);
    BuiltinType FloatTy(TYfloat);

    CanQualType CanonInt = CanQualType::create(QualType(&IntTy));
    CanQualType CanonIntDuplicate = CanQualType::create(QualType(&IntTy));
    CanQualType CanonFloat = CanQualType::create(QualType(&FloatTy));

    EXPECT_EQ(CanonInt, CanonIntDuplicate);
    EXPECT_NE(CanonInt, CanonFloat);
}
