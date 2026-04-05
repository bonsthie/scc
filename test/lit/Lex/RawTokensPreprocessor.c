// RUN: @scc_cc1 -dump-raw-tokens %s | FileCheck %s --check-prefix=RAW

// RAW: pp_hash '#'
// RAW: pp_define 'define'
// RAW: identifier 'VALUE'
// RAW: numeric_constant '42'
// RAW: eof
#define VALUE 42
VALUE
