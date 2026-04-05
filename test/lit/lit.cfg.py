import os
import shlex

import lit.formats

config.name = "scc tests"
config.test_format = lit.formats.ShTest(True)

config.suffixes = ['.c']

scc = os.environ.get("SCC_LIT_SCC")
if scc:
    scc = shlex.quote(scc)
    config.substitutions.append(("@scc_cc1", scc + " -cc1"))
    config.substitutions.append(("@scc", scc))
