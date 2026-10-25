// RUN: @scc_cc1 -fsyntax-only %s
// RUN: printf 'int ((*f);\n' > %t.c
// RUN: not @scc_cc1 -fsyntax-only %t.c 2>&1 | FileCheck %s

int ((value));
int (*callback)();
int (*array)[];

// CHECK: error: expected ')'
// CHECK-NEXT: 1 | int ((*f);
