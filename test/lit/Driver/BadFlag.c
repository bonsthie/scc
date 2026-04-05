// RUN: not @scc_cc1 -bad-flag 2>&1 | FileCheck %s --check-prefix=BADFLAG

// BADFLAG: unknow flag '-bad-flag'
int unused;
