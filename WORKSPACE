workspace(name="org_mookerji_sparse_direct")

new_http_archive(
    name="gtest",
    url="https://github.com/google/googletest/archive/release-1.7.0.zip",
    sha256="b58cb7547a28b2c718d1e38aee18a3659c9e3ff52440297e965f5edffe34b6d0",
    build_file="third_party/gtest.BUILD",
    strip_prefix="googletest-release-1.7.0",
)

new_http_archive(
    name="eigen",
    build_file="third_party/eigen.BUILD",
    sha256="7e05dd4b9866ef0aa4498be34752a362596cc5db2f8439cee111e4ea54046b57",
    strip_prefix="eigen-eigen-1d454915237a",
    url="http://bitbucket.org/eigen/eigen/get/1d454915237a.tar.gz",
)

new_http_archive(
    name="csparse",
    build_file="third_party/csparse.BUILD",
    strip_prefix="SuiteSparse",
    url=
    "http://faculty.cse.tamu.edu/davis/SuiteSparse/SuiteSparse-5.1.0.tar.gz",
)
