// RUN: @scc_cc1 -dump-tokens %s | FileCheck %s --check-prefix=TOKENS

// TOKENS: t_int 'int'
// TOKENS: identifier 'main'
// TOKENS: kw_return 'return'
// TOKENS: numeric_constant '0'
// TOKENS: eof
int main(void) { return 0; }
