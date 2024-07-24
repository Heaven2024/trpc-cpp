licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "validator_server_filter",
    srcs = ["validator_server_filter.cc"],
    hdrs = ["validator_server_filter.h"],

    deps = [
        "//trpc/filter",
        "//trpc/log:trpc_log",
        "//trpc/server:server_context",
        "//trpc/filter:filter_manager",
    ],
)