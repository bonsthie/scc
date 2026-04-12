// RUN: not @scc_cc1 -fsyntax-only %s 2>&1 | FileCheck %s

typedef foo foo;
typedef bar;

// CHECK: {{.*}}DeclSpecType.c:3:9: error: unknown type name 'foo'
// CHECK-NEXT: {{.*}}DeclSpecType.c:4:9: error: type specifier missing, defaults to 'int'; ISO C99 and later do not support implicit int [-Wimplicit-int]
