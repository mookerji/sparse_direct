.PHONY: cpp-test
cpp-test:
	bazel test cpp/... --test_output=all

.PHONY: clang-format-all
clang-format-all:
	git ls-files -- './*.cc' './*.h' | xargs clang-format -i

.PHONY: bazel-format-all
bazel-format-all:
	git ls-files -- '*.BUILD' 'WORKSPACE' | xargs yapf -i
