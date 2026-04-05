// RUN: @scc_cc1 -dump-raw-tokens fakefile.c %s 2>&1 | FileCheck %s

// CHECK: multiple input files specified; using "{{.*wrongFileCount\.c}}" and ignoring previous inputs
int foo() { return 1; }
