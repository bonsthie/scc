// RUN: not @scc_cc1 -fsyntax-only %s 2>&1 | FileCheck %s

typedef auto int typedef_auto_identifier;
unsigned long short int long_short_identifier;
unsigned long long long int long_long_long_identifier;

// CHECK: {{.*}}DeclSpecConflicts.c:3:9: error: cannot combine 'auto' with previous 'typedef' declaration specifier
// CHECK-NEXT: {{.*}}DeclSpecConflicts.c:4:15: error: cannot combine 'short' with previous 'long' declaration specifier
// CHECK-NEXT: {{.*}}DeclSpecConflicts.c:5:20: error: cannot combine 'long' with previous 'long long' declaration specifier
