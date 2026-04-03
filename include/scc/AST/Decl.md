

 ┌────────────────┐
 │ Decl           │ (Base class for everything)
 └──────┬─────────┘
        │
 ┌──────▼─────────┐
 │ NamedDecl      │ (Adds the ability to have a name, like "x")
 └──────┬─────────┘
        │
 ┌──────▼─────────┐
 │ ValueDecl      │ (Adds the ability to have a Type)
 └──────┬─────────┘
        │
 ┌──────▼─────────┐
 │ DeclaratorDecl │ (Handles C syntax like pointers, arrays, etc.)
 └──────┬─────────┘
        │
        ├─────────────────────────────┐
        │                             │
 ┌──────▼─────────┐            ┌──────▼─────────┐
 │ VarDecl        │            │ FieldDecl      │
 │ (Gets memory,  │            │ (Gets offsets, │
 │  has 'static') │            │  bit-fields)   │
 └────────────────┘            └────────────────┘
