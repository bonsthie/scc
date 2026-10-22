// RUN: not @scc_cc1 -fsyntax-only %s 2>&1 | FileCheck %s

int 123;

// CHECK: ExpectedDeclarator.c:3:5: error: expected identifier or '('
// CHECK-NEXT: 3 | int 123;
// CHECK-NEXT:   |     ^~~
