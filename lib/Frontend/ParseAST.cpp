#include "scc/Frontend/FrontendAction.h"

#include "scc/AST/ArrayType.h"
#include "scc/AST/BuiltinType.h"
#include "scc/AST/TagDecl.h"
#include "scc/AST/TagType.h"
#include "scc/AST/TypedefType.h"

using namespace scc;

bool scc::ParseAST(Parser &, ASTConsumer &Consumer, ASTContext &) {
    BuiltinType IntTy(TYint);
    BuiltinType LongTy(TYlong);
    ArrayType   LongArrayTy(&LongTy, 0);

    RecordFieldDecl Fields[] = {
        {QualType(&IntTy), std::string_view("lhs")},
        {QualType(&IntTy), std::string_view("rhs")},
        {QualType(&LongArrayTy), std::string_view("tail")},
    };

    RecordDecl Record(Struct, std::nullopt, Fields, 3);
    RecordType RecordTy(&Record);
    InlineRecordType InlineRecordTy(&Record);

    Qualifiers RecordQuals{};
    RecordQuals.IsVolatile = true;
    TypedefDecl Alias(std::string_view("VolatileStruct"), QualType(&RecordTy, RecordQuals));
    TypedefType AliasTy(&Alias);

    RecordFieldDecl Fields2[] = {
        {QualType(&RecordTy), std::string_view("struc")},
        {QualType(&InlineRecordTy), std::string_view("inline_struc")},
        {QualType(&AliasTy), std::string_view("alias")},
        {QualType(&IntTy), std::string_view("lhs")},
        {QualType(&IntTy), std::string_view("rhs")},
        {QualType(&LongArrayTy), std::string_view("tail")},
    };

    RecordDecl Record2(Struct, "second_strcut", Fields2, 3);

    for (Decl *D : {static_cast<Decl *>(&Record2), static_cast<Decl *>(&Alias)}) {
        Consumer.HandleTopLevelDecl(D);
    }

    return false;
}
