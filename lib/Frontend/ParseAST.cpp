#include "scc/AST/Decl.h"
#include "scc/Frontend/FrontendAction.h"

#include "scc/AST/BuiltinType.h"
#include "scc/AST/PointerType.h"
#include "scc/AST/TagDecl.h"
#include "scc/AST/TagType.h"
#include "scc/AST/TypedefType.h"

using namespace scc;

bool scc::ParseAST(Parser &P, ASTConsumer &Consumer) {

    while (!P.isEOF()) {
        if (Decl *D = P.parseTopLevelDecl())
            Consumer.handleTopLevelDecl(D);
    }

    return P.hasErrorOccurred();

    // BuiltinType IntTy(TYint);
    //
    // RecordFieldDecl TestFields[] = {
    //     {QualType(&IntTy), std::string_view("a")},
    // };
    // RecordDecl  TestRecord(Struct, std::nullopt, TestFields, 1);
    // RecordType  TestRecordTy(&TestRecord);
    // TypedefDecl TestTypedef(std::string_view("test"), QualType(&TestRecordTy));
    // TypedefType TestTy(&TestTypedef);
    //
    // RecordDecl FoobarDecl(Union, std::string_view("foobar"), nullptr, 0);
    // RecordType FoobarTy(&FoobarDecl);
    //
    // RecordFieldDecl InlineFields[] = {
    //     {QualType(&IntTy), std::string_view("bar")},
    // };
    // RecordDecl       InlineRecord(Struct, std::nullopt, InlineFields, 1);
    // InlineRecordType InlineRecordTy(&InlineRecord);
    //
    // RecordDecl  FooDecl(Struct, std::string_view("foo"), nullptr, 0);
    // RecordType  FooTy(&FooDecl);
    // PointerType FooPtrTy(&FooTy);
    //
    // RecordFieldDecl FooFields[] = {
    //     {QualType(&IntTy), std::string_view("foo")}, {QualType(&FooPtrTy),
    //     std::string_view("ptr")}, {QualType(&TestTy), std::string_view("a")},
    //     {QualType(&FoobarTy), std::string_view("yes")}, {QualType(&InlineRecordTy),
    //     std::nullopt},
    // };
    // FooDecl.setFields(FooFields, 5);
    //
    // for (Decl *D : {static_cast<Decl *>(&TestRecord), static_cast<Decl *>(&TestTypedef),
    //                 static_cast<Decl *>(&FoobarDecl), static_cast<Decl *>(&FooDecl)})
    //     Consumer.HandleTopLevelDecl(D);
    //
    // return false;
}
