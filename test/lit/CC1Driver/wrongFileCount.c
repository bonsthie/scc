// RUN: @scc_cc1 -dump-raw-tokens fakefile.c %s 2>&1 | FileCheck %s

// CHECK: multiple input files specified; using "{{.*wrongFileCount\.c}}" and ignoring previous inputs
// CHECK-NEXT: t_int 'int'
// CHECK-NEXT: identifier 'foo'
// CHECK-NEXT: l_paren '('
// CHECK-NEXT: r_paren ')'
// CHECK-NEXT: l_brace '{'
// CHECK-NEXT: kw_return 'return'
// CHECK-NEXT: numeric_constant '1'
// CHECK-NEXT: semi ';'
// CHECK-NEXT: r_brace '}'
// CHECK-NEXT: eof
int foo() { return 1; }
