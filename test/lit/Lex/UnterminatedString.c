// RUN: @scc_cc1 -dump-raw-tokens %s 2>&1 | FileCheck %s --check-prefix=STRINGERR

// STRINGERR: missing closing '"'
// STRINGERR: eof
"unterminated
