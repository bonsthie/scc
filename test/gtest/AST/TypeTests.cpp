#include "gtest/gtest.h"

#include "scc/AST/BuiltinType.h"
#include "scc/AST/CanQualType.h"
#include "scc/AST/QualType.h"
#include "scc/AST/RecordType.h"
#include "scc/AST/Type.h"
#include "scc/AST/TypedefType.h"

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

TEST(TypeTest, RecordTypeExposesElementsAsSpan) {
    BuiltinType IntTy(TYint);
    BuiltinType FloatTy(TYfloat);

    RecordType::Element Fields[] = {
        {QualType(&IntTy), std::string_view("lhs")},
        {QualType(&FloatTy), std::nullopt},
        {QualType(&IntTy), std::string_view("rhs")},
    };

    RecordStruct Record(std::string_view("Pair"), Fields, 3);

    EXPECT_EQ(TypeKind::Record, Record.kind());
    EXPECT_TRUE(Record.isRecordType());
    EXPECT_TRUE(Record.isStruct());
    EXPECT_FALSE(Record.isUnion());
    EXPECT_FALSE(Record.isAnonymous());
    ASSERT_TRUE(Record.getName().has_value());
    EXPECT_EQ(*Record.getName(), "Pair");
    EXPECT_FALSE(Record.empty());
    ASSERT_EQ(Record.size(), 3u);

    auto Elements = Record.getElements();
    ASSERT_EQ(Elements.size(), 3u);
    EXPECT_EQ(Elements.data(), Fields);
    EXPECT_EQ(Elements[0].Ty.getType(), &IntTy);
    ASSERT_TRUE(Elements[0].Name.has_value());
    EXPECT_EQ(*Elements[0].Name, "lhs");
    EXPECT_EQ(Elements[1].Ty.getType(), &FloatTy);
    EXPECT_FALSE(Elements[1].Name.has_value());
    EXPECT_EQ(Elements[2].Ty.getType(), &IntTy);
    ASSERT_TRUE(Elements[2].Name.has_value());
    EXPECT_EQ(*Elements[2].Name, "rhs");
}

TEST(TypeTest, RecordTypeSetElementsRebindsSpanView) {
    BuiltinType IntTy(TYint);
    BuiltinType FloatTy(TYfloat);

    RecordType::Element Initial[] = {{QualType(&IntTy), std::string_view("x")}};
    RecordType::Element Updated[] = {
        {QualType(&FloatTy), std::string_view("first")},
        {QualType(&IntTy), std::nullopt},
    };

    RecordUnion Record(Initial, 1);
    EXPECT_TRUE(Record.isUnion());
    EXPECT_FALSE(Record.isStruct());
    EXPECT_TRUE(Record.isAnonymous());
    Record.setElements(Updated, 2);
    Record.setName(std::string_view("Renamed"));

    auto Elements = Record.getElements();
    ASSERT_TRUE(Record.getName().has_value());
    EXPECT_EQ(*Record.getName(), "Renamed");
    ASSERT_EQ(Elements.size(), 2u);
    EXPECT_EQ(Elements.data(), Updated);
    EXPECT_EQ(Elements[0].Ty.getType(), &FloatTy);
    ASSERT_TRUE(Elements[0].Name.has_value());
    EXPECT_EQ(*Elements[0].Name, "first");
    EXPECT_EQ(Elements[1].Ty.getType(), &IntTy);
    EXPECT_FALSE(Elements[1].Name.has_value());
}
