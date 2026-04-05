// RUN: @scc_cc1 -dump-tokens -dump-ast %s 2>&1 | FileCheck %s --check-prefix=CONFLICT

// CONFLICT: -dump-ast action ignored -dump-tokens action specified previously
int main(void) { return 0; }
