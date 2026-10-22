// RUN: @scc_cc1 -fsyntax-only %s 2>&1 | FileCheck %s --check-prefix=WARN
// RUN: @scc_cc1 -fsyntax-only -Wno-duplicate-decl-specifier %s 2>&1 | FileCheck %s --check-prefix=QUIET
// RUN: @scc_cc1 -fsyntax-only -Wno-duplicate-decl-specifier -Wduplicate-decl-specifier %s 2>&1 | FileCheck %s --check-prefix=WARN
// RUN: @scc_cc1 -fsyntax-only -Wduplicate-decl-specifier -Wno-duplicate-decl-specifier %s 2>&1 | FileCheck %s --check-prefix=QUIET

int const const value;
int * volatile volatile pointer;

// WARN: warning: duplicate 'const' declaration specifier [-Wduplicate-decl-specifier]
// WARN-NEXT: 6 | int const const value;
// WARN-NEXT:   |           ^~~~~
// WARN: warning: duplicate 'volatile' declaration specifier [-Wduplicate-decl-specifier]
// WARN-NEXT: 7 | int * volatile volatile pointer;
// WARN-NEXT:   |                ^~~~~~~~

// QUIET-NOT: warning: duplicate
// QUIET: ParsedDeclarator
// QUIET-NOT: warning: duplicate
// QUIET: pointer volatile
// QUIET-NOT: warning: duplicate
