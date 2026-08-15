// RUN: @scc_cc1 -dump-tokens %s | FileCheck %s --check-prefix=TOKENS

// TOKENS: t_int 'int'
// TOKENS-NEXT: identifier 'main'
// TOKENS-NEXT: l_paren '('
// TOKENS-NEXT: t_void 'void'
// TOKENS-NEXT: r_paren ')'
// TOKENS-NEXT: l_brace '{'
// TOKENS-NEXT: kw_return 'return'
// TOKENS-NEXT: numeric_constant '0'
// TOKENS-NEXT: semi ';'
// TOKENS-NEXT: r_brace '}'
// TOKENS-NEXT: eof
int main(void) { return 0; }
