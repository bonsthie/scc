// RUN: @scc_cc1 -dump-raw-tokens --std=c89 %s 2>&1 | FileCheck %s --check-prefix=C89
// RUN: @scc_cc1 -dump-raw-tokens --std=c99 %s 2>&1 | FileCheck %s --check-prefix=C99
// RUN: @scc_cc1 -dump-raw-tokens --std=c99 -ftrigraphs %s 2>&1 | FileCheck %s --check-prefix=F-ON
// RUN: @scc_cc1 -dump-raw-tokens --std=c89 -fno-trigraphs %s 2>&1 | FileCheck %s --check-prefix=F-OFF
// RUN: @scc_cc1 -dump-raw-tokens --std=c99 -Wno-trigraphs -Wtrigraphs %s 2>&1 | FileCheck %s --check-prefix=MIX-WARN
// RUN: @scc_cc1 -dump-raw-tokens --std=c89 -Wtrigraphs -Wno-trigraphs %s 2>&1 | FileCheck %s --check-prefix=MIX-NO-WARN

// C89-NOT: warning: trigraph converted to '#' character [-Wtrigraphs]
// C89-NOT: question '?'
// C89: pp_hash '#'
// C89-NEXT: eof
// C89-NOT: warning: trigraph converted to '#' character [-Wtrigraphs]
// C89-NOT: question '?'

// C99-NOT: pp_hash '#'
// C99: warning: trigraph ignored [-Wtrigraphs]
// C99-NEXT: question '?'
// C99-NEXT: question '?'
// C99-NEXT: equal '='
// C99-NEXT: eof
// C99-NOT: pp_hash '#'

// F-ON: warning: trigraph converted to '#' character [-Wtrigraphs]
// F-ON-NEXT: pp_hash '#'
// F-ON-NEXT: eof
// F-ON-NOT: question '?'

// F-OFF-NOT: pp_hash '#'
// F-OFF: question '?'
// F-OFF-NEXT: question '?'
// F-OFF-NEXT: equal '='
// F-OFF-NEXT: eof
// F-OFF-NOT: pp_hash '#'

// MIX-WARN-NOT: pp_hash '#'
// MIX-WARN: warning: trigraph ignored [-Wtrigraphs]
// MIX-WARN-NEXT: question '?'
// MIX-WARN-NEXT: question '?'
// MIX-WARN-NEXT: equal '='
// MIX-WARN-NEXT: eof
// MIX-WARN-NOT: pp_hash '#'

// MIX-NO-WARN-NOT: warning: trigraph ignored [-Wtrigraphs]
// MIX-NO-WARN-NOT: question '?'
// MIX-NO-WARN: pp_hash '#'
// MIX-NO-WARN-NEXT: eof
// MIX-NO-WARN-NOT: warning: trigraph ignored [-Wtrigraphs]
// MIX-NO-WARN-NOT: question '?'

??=
