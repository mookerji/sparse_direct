licenses(["notice"])

# Modified from https://github.com/mzhaom/trunk/blob/master/third_party/gtest/BUILD

###########

cc_library(
    name="gtest",
    srcs=glob(
        ["src/*.cc"],
        exclude=["src/gtest-all.cc"],
    ),
    hdrs=glob([
        "include/**/*.h",
        "src/*.h",
    ]),
    includes=[
        "include",
    ],
    visibility=["//visibility:public"],
)

cc_library(
    name="gtest_main",
    srcs=["src/gtest_main.cc"],
    visibility=["//visibility:public"],
    deps=[":gtest"],
)
