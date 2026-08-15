// RUN: @scc_cc1 -dump-raw-tokens %s | FileCheck %s --check-prefix=RAW

// RAW: pp_hash '#'
// RAW-NEXT: pp_define 'define'
// RAW-NEXT: identifier 'VALUE'
// RAW-NEXT: numeric_constant '42'
// RAW-NEXT: identifier 'VALUE'
// RAW-NEXT: eof
#define VALUE 42
VALUE
