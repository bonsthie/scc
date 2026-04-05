#!/usr/bin/env bash

set -euo pipefail

if [[ $# -ne 1 ]]; then
  echo "usage: $0 <test-dir>" >&2
  exit 2
fi

if ! command -v lit >/dev/null 2>&1; then
  echo "lit not found on PATH" >&2
  exit 1
fi

workspace_root="${TEST_SRCDIR}/${TEST_WORKSPACE}"
test_dir="${workspace_root}/$1"
scc_bin="${workspace_root}/tools/scc/scc"

if [[ ! -d "${test_dir}" ]]; then
  echo "lit test directory not found: ${test_dir}" >&2
  exit 1
fi

if [[ ! -x "${scc_bin}" ]]; then
  echo "scc binary not found: ${scc_bin}" >&2
  exit 1
fi

export SCC_LIT_SCC="${scc_bin}"

exec lit -sv "${test_dir}"
