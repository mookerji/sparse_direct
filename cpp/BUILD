# LGPLv2.1 for csparse

licenses(["notice"])

cc_library(
    name="csparse",
    srcs=[
        "csparse.c",
    ],
    hdrs=[
        "csparse.h",
    ],
    visibility=["//visibility:public"],
)


cc_test(
    name = "test_proj1",
    srcs = ["test_proj1.cc"],
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:gtest_main",
        ":csparse",
    ],
)
