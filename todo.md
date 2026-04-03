## Core Infrastructure

* [x] `Token`
* [x] `Lexer`
* [x] `SourceLoc`
* [x] `SourceRange`
* [x] `SourceManager`
* [x] `Diagnostics`
* [x] `BumpArena` (slabbed bump allocator)
* [x] `StringInterner`
* [x] `Args Manager`
* [x] `Error Manger`
* [x] `FileManager`
* [ ] `PreProcessor` (base is done)


## AST Ownership / Factories

* [ ] `ASTContext` (arena owner, type uniquing, builtin caches)
* [ ] `ASTBuilder` (creates `Decl`/`Stmt`/`Expr`)
* [ ] `TypeBuilder` (folds `SpecSeq + Declarator` → `QualType`)

## Types

* [x] `Type` (base)
    * [x] `BuiltinType`
    * [ ] `PointerType`
    * [x] `ArrayType`
    * [ ] `FunctionType`
    * [x] `TagType` (base for tag-backed types)
        * [x] `RecordType` (struct/union)
        * [x] `EnumType`
    * [x] `TypedefType` (sugar / alias type)
    * [ ] `AtomicType`
* [x] `QualType` (wrapper: `Type*` + qualifier bits)

## Declarations

* [x] `Decl` (base)
    * [ ] `StaticAssertDecl`
    * [ ] `NamedDecl`
        * [ ] `TypeDecl`
            * [ ] `TypedefDecl`
            * [x] `TagDecl` (base for struct/union/enum)
                * [x] `RecordDecl` (struct/union)
                * [x] `EnumDecl`
        * [ ] `ValueDecl`
            * [ ] `VarDecl`
            * [ ] `ParmVarDecl`
            * [ ] `FunctionDecl`
            * [ ] `FieldDecl`
            * [ ] `EnumeratorDecl`

## Statements & Expressions (starter set)

* [ ] `Stmt` (base)
* [ ] `Expr` (base)
* [ ] `IntegerLiteral`
* [ ] `FloatingLiteral`
* [ ] `StringLiteral`
* [ ] `DeclRefExpr`
* [ ] `ArraySubscriptExpr`
* [ ] `MemberExpr`
* [ ] `UnaryExpr`
* [ ] `BinaryExpr`
* [ ] `ConditionalExpr`
* [ ] `CallExpr`
* [ ] `CastExpr`
* [ ] `InitListExpr`
* [ ] `CompoundStmt`
* [ ] `IfStmt`
* [ ] `WhileStmt`
* [ ] `ForStmt`
* [ ] `DoStmt`
* [ ] `ReturnStmt`
* [ ] `SwitchStmt` / `CaseStmt` / `DefaultStmt`
* [ ] `BreakStmt` / `ContinueStmt`
* [ ] `GotoStmt` / `LabelStmt`
* [ ] `DeclStmt` (wraps local declarations)

## Parser Layer

* [ ] `Parser` (owns `Tok`, `consume()`, `expect()`)
* [ ] `SpecSeq` (storage, func-spec, type-qual, type-spec, align, attrs)
* [ ] `Declarator`
* [ ] `PtrLayer`
* [ ] `ArraySuffix`
* [ ] `FunctionSuffix`
* [ ] `Attribute` (placeholder for C23/GNU/MSVC)
* [ ] `parseTranslationUnit()`
* [ ] `parseExternalDeclaration()`
* [ ] `parseDeclaration()`
* [ ] `parseFunctionDefinition()`
* [ ] `parseDeclSpecifiers()`
* [ ] `parseDeclarator()`
* [ ] `parseParamDeclaration()`
* [ ] `parseTypeName()` (casts/sizeof/compound literal)
* [ ] `parseAttributes()` (C23 `[[...]]`, GNU `__attribute__`, MS `__declspec`)
* [ ] `parseStmt()` family
* [ ] `parseExpr()` (precedence climbing / Pratt)
* [ ] `parseCompoundStmt()`
* [ ] `isDeclStart()` (incl. typedef-name lookup)

## Optional / Later

* [ ] `Attr` hierarchy (C23/GNU/MSVC)
* [ ] `SymbolTable`
* [ ] `Scope`
* [ ] `Sema` (ActOn* hooks, typedef-name tracking, constant-eval)
* [ ] `ASTVisitor`
* [ ] `ASTPrinter` / `ASTDumper`

---
