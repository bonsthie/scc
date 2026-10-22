// RUN: mkdir -p %t
// RUN: printf 'int const const xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx;\n' > %t/start.c
// RUN: printf 'int const %32sconst xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx;\n' '' > %t/middle.c
// RUN: printf 'int const %32sconst x;\n' '' > %t/end.c
// RUN: printf 'int 11111111111111111111111111111111111111111111111111111111111111111111111111111111;\n' > %t/long-token.c
// RUN: printf '/* unterminated\ncomment\n' > %t/comment.c
// RUN: env COLUMNS=32 @scc_cc1 -fsyntax-only %t/start.c 2>&1 | FileCheck %s --check-prefix=START --strict-whitespace
// RUN: env COLUMNS=32 @scc_cc1 -fsyntax-only %t/middle.c 2>&1 | FileCheck %s --check-prefix=MIDDLE --strict-whitespace
// RUN: env COLUMNS=32 @scc_cc1 -fsyntax-only %t/end.c 2>&1 | FileCheck %s --check-prefix=END --strict-whitespace
// RUN: env COLUMNS=32 not @scc_cc1 -fsyntax-only %t/long-token.c 2>&1 | FileCheck %s --check-prefix=LONG --strict-whitespace
// RUN: env COLUMNS=13 @scc_cc1 -fsyntax-only %t/middle.c 2>&1 | FileCheck %s --check-prefix=NARROW --strict-whitespace
// RUN: env COLUMNS=1 @scc_cc1 -fsyntax-only %t/middle.c 2>&1 | FileCheck %s --check-prefix=TINY --strict-whitespace

// Tabs, CRLF, and a line number with two digits must preserve alignment.
// RUN: printf '\n\n\n\n\n\n\n\n\n\n\n\tint\tconst\tconst\tvalue;\r\n' > %t/tabs.c
// RUN: env COLUMNS=32 @scc_cc1 -fsyntax-only %t/tabs.c 2>&1 | FileCheck %s --check-prefix=TABS --strict-whitespace

// Keep the EOF caret visible both with and without a final newline.
// RUN: printf 'int very_long_identifier_that_reaches_past_the_terminal_width' > %t/eof.c
// RUN: env COLUMNS=32 not @scc_cc1 -fsyntax-only %t/eof.c 2>&1 | FileCheck %s --check-prefix=EOF --strict-whitespace
// RUN: printf '\n' >> %t/eof.c
// RUN: env COLUMNS=32 not @scc_cc1 -fsyntax-only %t/eof.c 2>&1 | FileCheck %s --check-prefix=EOF-NL --strict-whitespace
// RUN: env COLUMNS=32 not @scc_cc1 -fsyntax-only %t/comment.c 2>&1 | FileCheck %s --check-prefix=COMMENT --strict-whitespace

// Color only the offending token; escape sequences must not affect cropping.
// RUN: env COLUMNS=32 @scc_cc1_color -fsyntax-only %t/middle.c 2>&1 | sed 's/\x1b/<ESC>/g' | FileCheck %s --check-prefix=COLOR --strict-whitespace

// A real terminal's width must take precedence over COLUMNS.
// RUN: script -qec "stty cols 32; env COLUMNS=11 @scc_cc1 -fsyntax-only %t/middle.c" /dev/null | FileCheck %s --check-prefix=MIDDLE --strict-whitespace

// START: warning: duplicate 'const'
// START-NEXT: {{^}}1 | int const const xxxxxxxxx...{{$}}
// START-NEXT: {{^}}  |           ^~~~~{{$}}
// MIDDLE: warning: duplicate 'const'
// MIDDLE-NEXT: {{^}}1 | ...        const xxxxxxxx...{{$}}
// MIDDLE-NEXT: {{^}}  |            ^~~~~{{$}}
// END: warning: duplicate 'const'
// END-NEXT: {{^}}1 | ...                 const x;{{$}}
// END-NEXT: {{^}}  |                     ^~~~~{{$}}
// LONG: error: expected identifier
// LONG-NEXT: {{^}}1 | ...1111111111111111111111...{{$}}
// LONG-NEXT: {{^}}  |    ^~~~~~~~~~~~~~~~~~~~~~{{$}}
// NARROW: warning: duplicate 'const'
// NARROW-NEXT: {{^}}1 | ...con...{{$}}
// NARROW-NEXT: {{^}}  |    ^~~{{$}}
// TINY: warning: duplicate 'const'
// TINY-NEXT: {{^}}c{{$}}
// TINY-NEXT: {{^}}^{{$}}
// TABS: warning: duplicate 'const'
// TABS-NEXT: {{^}}12 | ...const   const   value...{{$}}
// TABS-NEXT: {{^}}   |            ^~~~~{{$}}
// EOF: error:
// EOF-NEXT: {{^}}1 | ..._past_the_terminal_width{{$}}
// EOF-NEXT: {{^}}  |                            ^{{$}}
// EOF-NL: error:
// EOF-NL-NEXT: {{^}}2 | {{$}}
// EOF-NL-NEXT: {{^}}  | ^{{$}}
// COMMENT: error: unterminated /* comment
// COMMENT-NEXT: {{^}}1 | /* unterminated{{$}}
// COMMENT-NEXT: {{^}}  | ^~~~~~~~~~~~~~~{{$}}
// COLOR: {{^}}1 | ...        <ESC>[94mconst<ESC>[0m xxxxxxxx...{{$}}
// COLOR-NEXT: {{^}}  |            <ESC>[32m^~~~~<ESC>[0m{{$}}
