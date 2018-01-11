extern "C" {
#include "cs.h"
}
#include "gtest/gtest.h"

#include "Eigen/Core"
#include "Eigen/SparseCore"

// 2.1. Write a `cs_gatxpy` function that computes `y = A0x + y` without
// forming `A0`.
static int cs_gatxpy(const cs *A, const double *x, double *y) { return 0; }

TEST(Proj1, Ex2_1) { EXPECT_EQ("Hello Bazel", "Hello Bazel"); }

// 2.2. Write a function `cs_find` that converts a `cs` matrix into a
// triplet-form matrix, like the find function in MATLAB.
static int cs_find(const cs *A, const double *x, double *y) { return 0; }

TEST(Proj1, Ex2_2) { EXPECT_EQ("Hello Bazel", "Hello Bazel"); }

// 2.3. Write a variant of `cs_gaxpy` that computes `y = Ax + y`, where `A` is
// a symmetric matrix with only the upper triangular part present. Ignore
// entries in the lower triangular part.
static int cs_gaxpy_1(const cs *A, const double *x, double *y) { return 0; }

TEST(Proj1, Ex2_3) { EXPECT_EQ("Hello Bazel", "Hello Bazel"); }

// Expand the `Source/cs_gaxpy.c` function so that it can handle both
// compressed column and triplet matrices.
static int cs_gaxpy_2(const cs *A, const double *x, double *y) { return 0; }

TEST(Proj1, cs_gaxpy_cc_triplet) { EXPECT_EQ("Hello Bazel", "Hello Bazel"); }
