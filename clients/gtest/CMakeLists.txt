# ########################################################################
# Copyright (c) 2019-2021 Advanced Micro Devices, Inc.
# ########################################################################

# For debugging, uncomment this
# set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O0" )

# Linking lapack library requires fortran flags
enable_language( Fortran )
find_package( cblas REQUIRED CONFIG )

find_package( GTest REQUIRED )

set(roclapack_test_source
    # linear systems solvers
    getrs_gtest.cpp
    getri_gtest.cpp
    # least squares solvers
    gels_gtest.cpp
    # triangular factorizations
    getf2_getrf_gtest.cpp
    potf2_potrf_gtest.cpp
    # orthogonal factorizations
    geqr2_geqrf_gtest.cpp
    geql2_geqlf_gtest.cpp
    gelq2_gelqf_gtest.cpp
    # problem and matrix reductions (diagonalizations)
    gebd2_gebrd_gtest.cpp
    sytxx_hetxx_gtest.cpp
    sygsx_hegsx_gtest.cpp
    # singular value decomposition
    gesvd_gtest.cpp
    # symmetric eigensolvers
    syev_heev_gtest.cpp
    sygv_hegv_gtest.cpp
    )

set(rocauxiliary_test_source
    # vector & matrix manipulations
    lacgv_gtest.cpp
    laswp_gtest.cpp
    # householder reflections
    larf_gtest.cpp
    larfg_gtest.cpp
    larft_gtest.cpp
    larfb_gtest.cpp
    # orthonormal/unitary matrices
    orgxr_ungxr_gtest.cpp
    orglx_unglx_gtest.cpp
    orgxl_ungxl_gtest.cpp
    orgbr_ungbr_gtest.cpp
    orgtr_ungtr_gtest.cpp
    ormxr_unmxr_gtest.cpp
    ormlx_unmlx_gtest.cpp
    ormxl_unmxl_gtest.cpp
    ormbr_unmbr_gtest.cpp
    ormtr_unmtr_gtest.cpp
    # bidiagonal matrices
    labrd_gtest.cpp
    bdsqr_gtest.cpp
    # tridiagonal matrices
    sterf_gtest.cpp
    steqr_gtest.cpp
    latrd_gtest.cpp
    )

set(others_test_source
    # unified memory model
    managed_malloc_gtest.cpp
    )

set(rocsolver_test_source
    rocsolver_gtest_main.cpp
    )

add_executable( rocsolver-test ${roclapack_test_source} ${rocauxiliary_test_source} ${others_test_source} ${rocsolver_test_source} )

add_armor_flags( rocsolver-test "${ARMOR_LEVEL}" )

target_compile_definitions( rocsolver-test PRIVATE GOOGLE_TEST )

# External header includes included as system files
target_include_directories( rocsolver-test
  SYSTEM PRIVATE
    $<BUILD_INTERFACE:${HIP_INCLUDE_DIRS}>
    $<BUILD_INTERFACE:${CBLAS_INCLUDE_DIRS}>
    )

target_link_libraries( rocsolver-test PRIVATE
  cblas
  lapack
  GTest::GTest
  roc::rocsolver
)

if( CUDA_FOUND )
  target_include_directories( rocsolver-test
    PRIVATE
      $<BUILD_INTERFACE:${CUDA_INCLUDE_DIRS}>
      $<BUILD_INTERFACE:${hip_INCLUDE_DIRS}>
    )
  target_compile_definitions( rocsolver-test PRIVATE __HIP_PLATFORM_NVCC__ )
  target_link_libraries( rocsolver-test PRIVATE ${CUDA_LIBRARIES} )
else( )
  target_link_libraries( rocsolver-test PRIVATE hip::device )
endif( )

if( CMAKE_COMPILER_IS_GNUCXX OR CXX_VERSION_STRING MATCHES "clang")
  # GCC or hip-clang needs specific flags to turn on f16c intrinsics
  target_compile_options( rocsolver-test PRIVATE -mf16c )
endif( )

target_link_libraries( rocsolver-test PRIVATE m )
set_target_properties( rocsolver-test PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/staging"
)
target_compile_definitions( rocsolver-test PRIVATE ROCM_USE_FLOAT16 )

target_link_libraries( rocsolver-test PRIVATE rocsolver-common clients-common )
