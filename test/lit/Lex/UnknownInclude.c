// RUN: @scc_cc1 -dump-tokens %s 2>&1 | FileCheck %s --check-prefix=INCLUDE

// INCLUDE: can't find missing.h
// INCLUDE-NEXT: t_int 'int'
// INCLUDE-NEXT: identifier 'x'
// INCLUDE-NEXT: semi ';'
// INCLUDE-NEXT: eof
#include "missing.h"
int x;
