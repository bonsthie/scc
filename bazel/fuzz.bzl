load("@rules_fuzzing//fuzzing:cc_defs.bzl", "cc_fuzz_test")

def scc_cc_fuzz(name, deps, srcs = None, corpus = None, dicts = None):
    """Setup for a C++ fuzz target plus stable run/bin aliases."""

    if srcs == None:
        srcs = native.glob(["*.cpp"])

    if corpus == None:
        corpus = native.glob(["corpus/**"])

    if dicts == None:
        dicts = native.glob(["dict/**"])

    cc_fuzz_test(
        name = name,
        srcs = srcs,
        corpus = corpus,
        dicts = dicts,
        deps = deps,
    )

    native.alias(
        name = "run",
        actual = ":" + name + "_run",
    )

    native.alias(
        name = "bin",
        actual = ":" + name + "_bin",
    )
