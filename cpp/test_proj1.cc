extern "C" {
#include "cs.h"
}
#include "gtest/gtest.h"

#include "Eigen/Core"
#include "Eigen/SparseCore"

typedef Eigen::SparseMatrix<double> SpMat;
typedef Eigen::SparseMatrix<double> SpVec;
typedef Eigen::Matrix<double, Eigen::Dynamic, 1> ColumnVector;

#define DEBUG false

static cs *cs_eigen_to_cs(const Eigen::MatrixXf &mat, bool compress) {
  Eigen::ArrayXXf b(mat);
  const auto num_non_zero = (b > 0).count();
  cs *T = cs_spalloc(mat.rows(), mat.cols(), num_non_zero, 1, 1);
  for (int i = 0; i < mat.rows(); i++) {
    for (int j = 0; j < mat.cols(); j++) {
      cs_entry(T, i, j, mat(i, j));
    }
  }
  if (compress) {
    cs *sparse = cs_compress(T);
    cs_spfree(T);
    return sparse;
  } else {
    return T;
  }
}

template <class T> static void print_array(const T *arr, size_t size) {
  std::vector<T> vec(size);
  vec.assign(arr, arr + size);
  for (auto v : vec) {
    std::cout << v << ' ';
  }
  std::cout << std::endl;
}

// 2.1. Write a `cs_gatxpy` function that computes `y = ATx + y` without
// forming `A0`.
static csi cs_gatxpy(const cs *A, const double *x, double *y) {
  csi p, j, n, *Ap, *Ai;
  double *Ax;
  if (!CS_CSC(A) || !x || !y)
    return 0;
  n = A->n;
  Ap = A->p;
  Ai = A->i;
  Ax = A->x;
  for (j = 0; j < n; j++) {
    for (p = Ap[j]; p < Ap[j + 1]; p++) {
      y[Ai[p]] += Ax[p] * x[j];
    }
  }
  return 1;
}

TEST(Proj1, Ex2_1) {
  Eigen::MatrixXf A(5, 5);
  Eigen::MatrixXf x(5, 1);
  Eigen::MatrixXf y(5, 1);
  // clang-format off
  A << 0,3,0,0,0,
       22,0,0,0,17,
       7,5,0,1,0,
       0,0,0,0,0,
       0,0,14,0,8;
  x << 1,1,1,1,1;
  y << 1,1,1,1,1;
  // clang-format on
  const auto &yp = A * x + y;
  const auto &ytp = A.transpose() * x + y;
  if (DEBUG) {
    std::cout << "A: " << A << std::endl;
    std::cout << "AT: " << A.transpose() << std::endl;
    std::cout << "yp: " << yp << std::endl;
    std::cout << "ytp: " << ytp << std::endl;
  }
  // Eigen stuff
  cs *Acss = cs_eigen_to_cs(A, 1);
  double x0[] = {1, 1, 1, 1, 1};
  double y0[] = {1, 1, 1, 1, 1};
  cs_gaxpy(Acss, x0, y0);
  if (DEBUG) {
    print_array(y0, 5);
    cs_print(Acss, 0);
  }
  cs_spfree(Acss);
  EXPECT_EQ("Hello Bazel", "Hello Bazel");
}

// 2.2. Write a function `cs_find` that converts a `cs` matrix into a
// triplet-form matrix, like the find function in MATLAB.
static csi cs_find(const cs *A, csi *rows, csi *nrows, csi *cols, csi *ncols,
                   double *vals) {
  if (!CS_CSC(A)) {
    return 0;
  }
  // Rows and values are already in triplet order, so we only need to copy those
  // over directly and iterate through the sparse values to accumulate the
  // column values.
  //
  // Allocate and copy over the rows
  rows = (csi *)calloc(A->nzmax, sizeof(csi));
  *nrows = A->nzmax;
  memcpy(rows, A->i, A->nzmax);
  // Allocate and copy over the values
  vals = (double *)calloc(A->nzmax, sizeof(double));
  memcpy(vals, A->x, A->nzmax);
  // Allocate and iterate to get the columns
  cols = (csi *)calloc(A->nzmax, sizeof(csi));
  csi n = 0;
  for (csi j = 0; j < A->n; j++) {
    for (csi p = A->p[j]; p < A->p[j + 1]; p++) {
      if (A->x[p] > 0) {
        cols[n] = j;
        ++n;
      }
    }
  }
  *ncols = n - 1;
  return 1;
}

TEST(Proj1, Ex2_2) {
  Eigen::MatrixXf A(5, 5);
  // clang-format off
  A << 1,0,0,0,1,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,1,0,
       0,0,0,0,1;
  // clang-format on
  // Eigen stuff
  cs *Acss = cs_eigen_to_cs(A, 1);
  csi *rows = nullptr, *cols = nullptr;
  csi nrows = 0, ncols = 0;
  double *vals = nullptr;
  EXPECT_EQ(cs_find(Acss, rows, &nrows, cols, &ncols, vals), 1);
  EXPECT_EQ(ncols, 3);
  cs_spfree(Acss);
  free(rows);
  free(cols);
  free(vals);
}

// 2.3. Write a variant of `cs_gaxpy` that computes `y = Ax + y`, where `A` is
// a symmetric matrix with only the upper triangular part present. Ignore
// entries in the lower triangular part.
static inline bool is_upper_entry(csi row, csi col) { return row <= col; }

static csi cs_gaxpy_1(const cs *A, const double *x, double *y) {
  csi p, j, n, *Ap, *Ai;
  double *Ax;
  if (!CS_CSC(A) || !x || !y)
    return 0;
  n = A->n;
  Ap = A->p;
  Ai = A->i;
  Ax = A->x;
  csi row = 0, col = 0;
  for (j = 0; j < n; j++) {
    col = j;
    for (p = Ap[j]; p < Ap[j + 1]; p++) {
      row = Ai[p];
      if (is_upper_entry(row, col)) {
        y[Ai[p]] += Ax[p] * x[j];
      } else if (Ax[p] != 0) {
        // This should be zero! if actually upper triangular.
        return 0;
      }
    }
  }
  return 1;
}

TEST(Proj1, Ex2_3) {
  Eigen::MatrixXf A(5, 5);
  Eigen::MatrixXf x(5, 1);
  Eigen::MatrixXf y(5, 1);
  // clang-format off
  A << 1,0,0,0,1,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,1,0,
       0,0,0,0,1;
  x << 1,1,1,1,1;
  y << 1,1,1,1,1;
  // clang-format on
  const auto &yp = A * x + y;
  // Eigen stuff
  // Diagonal
  cs *Acss = cs_eigen_to_cs(A, 1);
  double x0[] = {1, 1, 1, 1, 1};
  double y0[] = {1, 1, 1, 1, 1};
  EXPECT_EQ(cs_gaxpy_1(Acss, x0, y0), 1);
  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(y0[i], yp(i, 0));
  }
  cs_spfree(Acss);
  // Not diagonal
  double z0[] = {1, 1, 1, 1, 1};
  // clang-format off
  A << 1,0,0,0,1,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,1,0,
       1,0,0,0,1;
  // clang-format on
  cs *Acst = cs_eigen_to_cs(A, 1);
  EXPECT_EQ(cs_gaxpy_1(Acst, x0, y0), 0);
  cs_spfree(Acst);
}

// Expand the `Source/cs_gaxpy.c` function so that it can handle both
// compressed column and triplet matrices.
static csi cs_gaxpy_2(const cs *A, const double *x, double *y) {
  if (!x || !y) {
    return 0;
  } else if (CS_CSC(A)) {
    return cs_gaxpy(A, x, y);
  }
  csi ncols = A->n, nrows = A->m, nelts = ncols * nrows;
  csi *row_index = A->i, *col_index = A->p;
  double *values = A->x;
  csi i = 0, j = 0;
  while (i < nelts && j < nelts) {
    csi row = row_index[i], col = col_index[j];
    csi k = row * ncols + col;
    if (values[k] > 0) {
      y[row] += values[k] * x[row];
    }
    ++i;
    ++j;
  }
  return 1;
}

TEST(Proj1, cs_gaxpy_cc_triplet) {
  Eigen::MatrixXf A(5, 5);
  Eigen::MatrixXf x(5, 1);
  Eigen::MatrixXf y(5, 1);
  // clang-format off
  A << 1,0,0,0,1,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,1,0,
       0,0,0,0,1;
  x << 1,1,1,1,1;
  y << 1,1,1,1,1;
  // clang-format on
  const auto &yp = A * x + y;
  // Eigen stuff
  double x0[] = {1, 1, 1, 1, 1};
  double y0[] = {1, 1, 1, 1, 1};
  // sparse
  cs *Acss = cs_eigen_to_cs(A, 1);
  EXPECT_EQ(cs_gaxpy_2(Acss, x0, y0), 1);
  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(y0[i], yp(i, 0));
  }
  cs_spfree(Acss);
  // triplet
  cs *Acst = cs_eigen_to_cs(A, 0);
  double z0[] = {1, 1, 1, 1, 1};
  EXPECT_EQ(cs_gaxpy_2(Acst, x0, z0), 1);
  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(z0[i], yp(i, 0));
  }
  cs_spfree(Acst);
}
