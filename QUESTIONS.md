## SAMPLE PROJECT 1

Four problems: Exercises 2.1, 2.2, 2.3 in the book and the problem
listed below.

- Expand the `Source/cs_gaxpy.c` function so that it can handle both
  compressed column and triplet matrices. Then create a mexFunction
  with the MATLAB prototype of: `z = cs_gaxpy_triplet (I, J, X, x, y)`
  where the vectors `I`, `J`, and `X` present a sparse matrix in
  triplet form (the kth nonzero in the matrix has row index `I(k)`,
  column index `J(k)`, and value `X(k)`). You can create `I`,`J`,`X`
  via `[I,J,X]=find(A)` where `A` is a sparse matrix.
- 2.1. Write a `cs_gatxpy` function that computes `y = A0x + y` without
  forming `A0`.
- 2.2. Write a function `cs_find` that converts a `cs` matrix into a
  triplet-form matrix, like the find function in MATLAB.
- 2.3. Write a variant of `cs_gaxpy` that computes `y = Ax + y`, where `A`
  is a symmetric matrix with only the upper triangular part
  present. Ignore entries in the lower triangular part.
