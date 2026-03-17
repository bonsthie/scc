


BumpAllocator 
 - mmap like 2G mmprotect so the memory is not allocated but will always be contigu
 - mmprotect block when needed


## Core Infrastructure

* [x] `Token`
    - [ ] trigraph ???
* [x] `Lexer`
* [x] `SourceLoc`
* [ ] `SourceRange`
* [x] `SourceManager`
* [x] `Diagnostics`
* [ ] `BumpArena` (slabbed bump allocator)
* [ ] `StringInterner`

## AST Ownership / Factories

* [ ] `ASTContext` (arena owner, type uniquing, builtin caches)
* [ ] `ASTBuilder` (creates `Decl`/`Stmt`/`Expr`)
* [ ] `TypeBuilder` (folds `SpecSeq + Declarator` → `QualType`)

## Types

* [ ] `QualType` (Type* + qualifier bits)
* [ ] `Type` (base)
* [ ] `BuiltinType`
* [ ] `PointerType`
* [ ] `ArrayType`
* [ ] `FunctionType`
* [ ] `RecordType` (struct/union)
* [ ] `EnumType`
* [ ] `AtomicType`

## Declarations

* [ ] `Decl` (base)
* [ ] `VarDecl`
* [ ] `ParmVarDecl`
* [ ] `FunctionDecl`
* [ ] `TypedefDecl`
* [ ] `RecordDecl` (struct/union)
* [ ] `FieldDecl`
* [ ] `EnumDecl`
* [ ] `EnumeratorDecl`
* [ ] `StaticAssertDecl`

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
