Type*
 ├── BuiltinType     (int, void, char…)
 ├── PointerType     (T*)
 ├── FunctionType    (T(...))
 ├── ArrayType       (T[N])
 ├── EnumType        (enum E)
 └── RecordType      (struct / union)

Qualifiers
 ├── const
 ├── volatile
 ├── restrict
 └── ...

QualType
  ├── Type* → RecordType → struct S
  └── qualifiers (const, volatile…)

CanQualType
  └── canonical QualType
      ├── no typedef / no sugar
      └── qualifiers preserved
