load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")
load("@rules_shell//shell:sh_test.bzl", "sh_test")

def scc_cc_lib(name, srcs = [], hdrs = [], deps = [], visibility = ["//visibility:public"], **kwargs):
    """A custom C++ library rule for the SCC compiler."""

    scc_deps = deps + ["//include:scc_headers"]

    cc_library(
        name = name,
        srcs = srcs,
        hdrs = hdrs,
        deps = scc_deps,
        visibility = visibility,
        **kwargs
    )

def scc_cc_binary(name, srcs = [], deps = [], visibility = ["//visibility:public"], **kwargs):
    """A custom C++ binary rule for SCC executables."""

    scc_deps = deps + ["//include:scc_headers"]

    cc_binary(
        name = name,
        srcs = srcs,
        deps = scc_deps,
        visibility = visibility,
        **kwargs
    )

def scc_cc_test(name, srcs = [], deps = [], data = [], visibility = ["//visibility:public"], **kwargs):
    """Define a gtest-backed C++ test plus a same-name suite target."""

    env = {
        "GTEST_BRIEF": "1",
        "GTEST_COLOR": "1",
    }
    env.update(kwargs.pop("env", {}))

    scc_deps = deps + [
        "//include:scc_headers",
        "@googletest//:gtest_main",
    ]

    cc_test(
        name = name + "_gtest",
        srcs = srcs,
        data = data,
        deps = scc_deps,
        env = env,
        visibility = visibility,
        **kwargs
    )

    native.test_suite(
        name = name,
        tests = [":" + name + "_gtest"],
        visibility = visibility,
    )

def scc_lit_test(name, dir = ".", data = [], visibility = ["//visibility:public"], **kwargs):
    """Define a lit-backed shell test plus a same-name suite target."""

    package = native.package_name()
    if package and dir != ".":
        test_root = package + "/" + dir
    elif package:
        test_root = package
    else:
        test_root = dir

    patterns = ["**"] if dir == "." else [dir + "/**"]
    lit_data = native.glob(
        patterns,
        exclude = [
            "BUILD",
            "BUILD.bazel",
            "**/BUILD",
            "**/BUILD.bazel",
            "**/*.script",
            "**/*.tmp",
            ".lit_test_times.txt",
        ],
    )

    sh_test(
        name = name + "_lit",
        srcs = ["//bazel:run_lit.sh"],
        args = [test_root],
        data = lit_data + data,
        visibility = visibility,
        **kwargs
    )

    native.test_suite(
        name = name,
        tests = [":" + name + "_lit"],
        visibility = visibility,
    )

def scc_test_group(name, tests = [], lit = None, gtest = None, visibility = ["//visibility:public"], **kwargs):
    """Aggregate lit and gtest suite targets under one label."""

    group_tests = list(tests)
    if lit:
        group_tests.append(lit)
    if gtest:
        group_tests.append(gtest)

    native.test_suite(
        name = name,
        tests = group_tests,
        visibility = visibility,
        **kwargs
    )
