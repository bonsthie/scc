// RUN: @scc_cc1 -dump-raw-tokens --std=c89 %s 2>&1 | FileCheck %s --check-prefix=C89
// RUN: @scc_cc1 -dump-raw-tokens --std=c99 %s 2>&1 | FileCheck %s --check-prefix=C99
// RUN: @scc_cc1 -dump-raw-tokens --std=c99 -Wno-trigraphs -Wtrigraphs %s 2>&1 | FileCheck %s --check-prefix=MIX-WARN
// RUN: @scc_cc1 -dump-raw-tokens --std=c89 -Wtrigraphs -Wno-trigraphs %s 2>&1 | FileCheck %s --check-prefix=MIX-NO-WARN

// C89-NOT: trigraph ignored [-Wtrigraphs]
// C89: pp_hash '#'
// C89-NOT: question '?'

// C99: trigraph ignored [-Wtrigraphs]
// C99: question '?'
// C99: question '?'
// C99: equal '='
// C99-NOT: pp_hash '#'

// MIX-WARN: trigraph ignored [-Wtrigraphs]
// MIX-WARN: question '?'
// MIX-WARN: question '?'
// MIX-WARN: equal '='
// MIX-WARN-NOT: pp_hash '#'

// MIX-NO-WARN-NOT: trigraph ignored [-Wtrigraphs]
// MIX-NO-WARN: pp_hash '#'
// MIX-NO-WARN-NOT: question '?'

??=
