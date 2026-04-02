#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/.." && pwd)"

usage() {
    cat <<'EOF'
Usage:
  fuzz/run.sh <target> [options] [-- <extra libFuzzer args>]

Targets:
  lex

Options:
  --time <seconds>         Max fuzzing time. Default: 600
  --jobs <n>               libFuzzer jobs. Default: 0 (unset)
  --workers <n>            libFuzzer workers. Default: 0 (unset)
  --artifact-dir <path>    Crash/artifact output dir. Default: temp dir
  --work-dir <path>        Working corpus dir. Default: temp dir
  --seed <path>            Extra file or directory copied into working corpus
  --keep-workdir           Keep temp working corpus after exit
  --dry-run                Print the final Bazel command without running it
  -h, --help               Show this help

Examples:
  fuzz/run.sh lex
  fuzz/run.sh lex --time 60 --jobs 8 --workers 8
  fuzz/run.sh lex --seed /path/to/file.c -- -max_len=8192
EOF
}

target_name="${1:-}"
if [[ -z "${target_name}" ]]; then
    usage
    exit 1
fi
shift

case "${target_name}" in
    lex)
        corpus_src="${SCRIPT_DIR}/lex/corpus"
        dict_file="${SCRIPT_DIR}/lex/dict/lex.dict"
        bazel_target="//fuzz/lex:bin"
        ;;
    -h|--help)
        usage
        exit 0
        ;;
    *)
        echo "unknown fuzz target: ${target_name}" >&2
        usage
        exit 1
        ;;
esac

time_limit=600
jobs=0
workers=0
artifact_dir=""
work_dir=""
dict_file="${dict_file:-}"
keep_workdir=0
dry_run=0
seeds=()
extra_args=()

while [[ $# -gt 0 ]]; do
    case "$1" in
        --time)
            time_limit="${2:?missing value for --time}"
            shift 2
            ;;
        --jobs)
            jobs="${2:?missing value for --jobs}"
            shift 2
            ;;
        --workers)
            workers="${2:?missing value for --workers}"
            shift 2
            ;;
        --artifact-dir)
            artifact_dir="${2:?missing value for --artifact-dir}"
            shift 2
            ;;
        --work-dir)
            work_dir="${2:?missing value for --work-dir}"
            shift 2
            ;;
        --seed)
            seeds+=("${2:?missing value for --seed}")
            shift 2
            ;;
        --keep-workdir)
            keep_workdir=1
            shift
            ;;
        --dry-run)
            dry_run=1
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        --)
            shift
            extra_args+=("$@")
            break
            ;;
        *)
            extra_args+=("$1")
            shift
            ;;
    esac
done

mkdir -p "${corpus_src}"

created_workdir=0
if [[ -z "${work_dir}" ]]; then
    work_dir="$(mktemp -d "${TMPDIR:-/tmp}/scc-fuzz-${target_name}-XXXXXX")"
    created_workdir=1
fi

created_artifact_dir=0
if [[ -z "${artifact_dir}" ]]; then
    artifact_dir="$(mktemp -d "${TMPDIR:-/tmp}/scc-fuzz-artifacts-${target_name}-XXXXXX")"
    created_artifact_dir=1
fi

cleanup() {
    if [[ "${created_workdir}" -eq 1 && "${keep_workdir}" -eq 0 ]]; then
        rm -rf "${work_dir}"
    fi
}
trap cleanup EXIT

mkdir -p "${work_dir}" "${artifact_dir}"
cp -R "${corpus_src}/." "${work_dir}/"

for seed in "${seeds[@]}"; do
    if [[ -d "${seed}" ]]; then
        cp -R "${seed}/." "${work_dir}/"
    elif [[ -f "${seed}" ]]; then
        cp "${seed}" "${work_dir}/"
    else
        echo "seed path does not exist: ${seed}" >&2
        exit 1
    fi
done

cmd=(
    bazel
    run
    --config=asan-libfuzzer
    "${bazel_target}"
    --
    "${work_dir}"
    "-max_total_time=${time_limit}"
    -print_final_stats=1
    "-artifact_prefix=${artifact_dir}/"
)

if [[ -n "${dict_file}" && -f "${dict_file}" ]]; then
    cmd+=("-dict=${dict_file}")
fi

if [[ "${jobs}" -gt 0 ]]; then
    cmd+=("-jobs=${jobs}")
fi

if [[ "${workers}" -gt 0 ]]; then
    cmd+=("-workers=${workers}")
fi

if [[ ${#extra_args[@]} -gt 0 ]]; then
    cmd+=("${extra_args[@]}")
fi

echo "Corpus source: ${corpus_src}"
if [[ -n "${dict_file}" && -f "${dict_file}" ]]; then
    echo "Dictionary: ${dict_file}"
fi
echo "Working corpus: ${work_dir}"
echo "Artifacts: ${artifact_dir}"
if [[ "${created_workdir}" -eq 1 && "${keep_workdir}" -eq 0 ]]; then
    echo "Working corpus will be deleted on exit. Use --keep-workdir to keep it."
fi
if [[ "${created_artifact_dir}" -eq 1 ]]; then
    echo "Artifact directory is temporary but kept after the run."
fi
printf 'Running:'
printf ' %q' "${cmd[@]}"
printf '\n'

if [[ "${dry_run}" -eq 1 ]]; then
    exit 0
fi

cd "${REPO_ROOT}"
exec "${cmd[@]}"
