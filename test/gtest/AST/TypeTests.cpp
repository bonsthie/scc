#include "gtest/gtest.h"

#include "scc/AST/BuiltinType.h"
#include "scc/AST/CanQualType.h"
#include "scc/AST/QualType.h"
#include "scc/AST/TagType.h"
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
    TypedefDecl AliasDecl("MyInt", QualType(&IntTy, UnderQuals));

    TypedefType Alias(&AliasDecl);
    QualType    Under = Alias.getUnderlyingType();

    EXPECT_EQ(&IntTy, Under.getType());
    EXPECT_TRUE(Under.isConstQualified());
    EXPECT_FALSE(Under.isVolatileQualified());
    EXPECT_FALSE(Under.isRestrictQualified());
    ASSERT_TRUE(Alias.getDecl()->getName().has_value());
    EXPECT_EQ(*Alias.getDecl()->getName(), "MyInt");
}

TEST(QualTypeTest, DesugarOnceMergesQualifiers) {
    BuiltinType IntTy(TYint);

    Qualifiers UnderQuals{};
    UnderQuals.IsVolatile = true;
    TypedefDecl AliasDecl("VolatileInt", QualType(&IntTy, UnderQuals));
    TypedefType Alias(&AliasDecl);

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
    TypedefDecl InnerDecl("Inner", QualType(&IntTy, InnerQuals));
    TypedefType Inner(&InnerDecl);

    Qualifiers MiddleQuals{};
    MiddleQuals.IsConst = true;
    TypedefDecl MiddleDecl("Middle", QualType(&Inner, MiddleQuals));
    TypedefType Middle(&MiddleDecl);

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

TEST(TypeTest, EnumTypeReferencesEnumDecl) {
    EnumFieldDecl Fields[] = {
        EnumFieldDecl(std::string_view("Red")),
        EnumFieldDecl(std::string_view("Green")),
    };
    EnumDecl Decl(Enum, std::string_view("Color"), Fields, 2);

    EnumType Ty(&Decl);

    EXPECT_EQ(TypeKind::Enum, Ty.kind());
    EXPECT_TRUE(Ty.isEnumType());
    EXPECT_FALSE(Ty.isRecordType());
    EXPECT_FALSE(Ty.isAnonymous());
    EXPECT_TRUE(Ty.isEnum());
    EXPECT_FALSE(Ty.isStruct());
    EXPECT_FALSE(Ty.isUnion());
    EXPECT_EQ(Ty.getDecl(), &Decl);
    ASSERT_TRUE(Ty.getDecl()->getName().has_value());
    EXPECT_EQ(*Ty.getDecl()->getName(), "Color");
}

TEST(TypeTest, RecordTypeExposesElementsAsSpan) {
    BuiltinType IntTy(TYint);
    BuiltinType FloatTy(TYfloat);

    RecordFieldDecl Fields[] = {
        {QualType(&IntTy), std::string_view("lhs")},
        {QualType(&FloatTy), std::nullopt},
        {QualType(&IntTy), std::string_view("rhs")},
    };
    RecordDecl Decl(Struct, std::string_view("Pair"), Fields, 3);

    RecordType Record(&Decl);

    EXPECT_EQ(TypeKind::Record, Record.kind());
    EXPECT_TRUE(Record.isRecordType());
    EXPECT_TRUE(Record.isStruct());
    EXPECT_FALSE(Record.isUnion());
    EXPECT_FALSE(Record.isAnonymous());
    ASSERT_TRUE(Decl.getName().has_value());
    EXPECT_EQ(*Decl.getName(), "Pair");
    EXPECT_FALSE(Decl.empty());
    ASSERT_EQ(Decl.size(), 3u);

    auto FieldsView = Decl.getFields();
    ASSERT_EQ(FieldsView.size(), 3u);
    EXPECT_EQ(FieldsView.data(), Fields);
    EXPECT_EQ(FieldsView[0].getType().getType(), &IntTy);
    ASSERT_TRUE(FieldsView[0].getName().has_value());
    EXPECT_EQ(*FieldsView[0].getName(), "lhs");
    EXPECT_EQ(FieldsView[1].getType().getType(), &FloatTy);
    EXPECT_FALSE(FieldsView[1].getName().has_value());
    EXPECT_EQ(FieldsView[2].getType().getType(), &IntTy);
    ASSERT_TRUE(FieldsView[2].getName().has_value());
    EXPECT_EQ(*FieldsView[2].getName(), "rhs");
}

TEST(TypeTest, RecordTypeSetElementsRebindsSpanView) {
    BuiltinType IntTy(TYint);
    BuiltinType FloatTy(TYfloat);

    RecordFieldDecl Initial[] = {{QualType(&IntTy), std::string_view("x")}};
    RecordFieldDecl Updated[] = {
        {QualType(&FloatTy), std::string_view("first")},
        {QualType(&IntTy), std::nullopt},
    };
    RecordDecl Decl(Union, std::nullopt, Initial, 1);

    RecordType Record(&Decl);
    EXPECT_TRUE(Record.isUnion());
    EXPECT_FALSE(Record.isStruct());
    EXPECT_TRUE(Record.isAnonymous());
    Decl.setFields(Updated, 2);
    Decl.setName(std::string_view("Renamed"));

    auto FieldsView = Decl.getFields();
    ASSERT_TRUE(Decl.getName().has_value());
    EXPECT_EQ(*Decl.getName(), "Renamed");
    ASSERT_EQ(FieldsView.size(), 2u);
    EXPECT_EQ(FieldsView.data(), Updated);
    EXPECT_EQ(FieldsView[0].getType().getType(), &FloatTy);
    ASSERT_TRUE(FieldsView[0].getName().has_value());
    EXPECT_EQ(*FieldsView[0].getName(), "first");
    EXPECT_EQ(FieldsView[1].getType().getType(), &IntTy);
    EXPECT_FALSE(FieldsView[1].getName().has_value());
}
