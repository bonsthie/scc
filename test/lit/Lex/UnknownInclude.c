// RUN: @scc_cc1 -dump-tokens %s 2>&1 | FileCheck %s --check-prefix=INCLUDE

// INCLUDE: can't find missing.h
// INCLUDE: t_int 'int'
// INCLUDE: identifier 'x'
// INCLUDE: eof
#include "missing.h"
int x;
