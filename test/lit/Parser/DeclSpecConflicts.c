// RUN: grep '^// AUTO-SRC:' %s | sed 's@// AUTO-SRC: @@' > %t.typedef-auto.c
// RUN: not @scc_cc1 -fsyntax-only %t.typedef-auto.c 2>&1 | FileCheck %s --check-prefix=AUTO
// RUN: grep '^// SHORT-SRC:' %s | sed 's@// SHORT-SRC: @@' > %t.long-short.c
// RUN: not @scc_cc1 -fsyntax-only %t.long-short.c 2>&1 | FileCheck %s --check-prefix=SHORT
// RUN: grep '^// LONG-SRC:' %s | sed 's@// LONG-SRC: @@' > %t.long-long-long.c
// RUN: not @scc_cc1 -fsyntax-only %t.long-long-long.c 2>&1 | FileCheck %s --check-prefix=LONG

// TODO this is temporary until we have a better parser

// AUTO: {{.*}}typedef-auto.c:1:9: error: cannot combine 'auto' with previous 'typedef' declaration specifier
// AUTO-NEXT: typedef int
// SHORT: {{.*}}long-short.c:1:15: error: cannot combine 'short' with previous 'long' declaration specifier
// SHORT-NEXT: unsigned long int
// LONG: {{.*}}long-long-long.c:1:20: error: cannot combine 'long' with previous 'long long' declaration specifier
// LONG-NEXT: unsigned long long int

// AUTO-SRC: typedef auto int typedef_auto_identifier;
// SHORT-SRC: unsigned long short int long_short_identifier;
// LONG-SRC: unsigned long long long int long_long_long_identifier;
