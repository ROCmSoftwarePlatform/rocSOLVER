/* ************************************************************************
 * Copyright (c) 2019-2021 Advanced Micro Devices, Inc.
 * ************************************************************************ */

#include "roclapack_gebd2.hpp"

template <typename S, typename T, typename U>
rocblas_status rocsolver_gebd2_strided_batched_impl(rocblas_handle handle,
                                                    const rocblas_int m,
                                                    const rocblas_int n,
                                                    U A,
                                                    const rocblas_int lda,
                                                    const rocblas_stride strideA,
                                                    S* D,
                                                    const rocblas_stride strideD,
                                                    S* E,
                                                    const rocblas_stride strideE,
                                                    T* tauq,
                                                    const rocblas_stride strideQ,
                                                    T* taup,
                                                    const rocblas_stride strideP,
                                                    const rocblas_int batch_count)
{
    ROCSOLVER_ENTER_TOP("gebd2_strided_batched", "-m", m, "-n", n, "--lda", lda, "--bsa", strideA,
                        "--bsp", strideP, "--batch", batch_count);

    if(!handle)
        return rocblas_status_invalid_handle;

    // argument checking
    rocblas_status st
        = rocsolver_gebd2_gebrd_argCheck(handle, m, n, lda, A, D, E, tauq, taup, batch_count);
    if(st != rocblas_status_continue)
        return st;

    // working with unshifted arrays
    rocblas_int shiftA = 0;

    // memory workspace sizes:
    // size for constants in rocblas calls
    size_t size_scalars;
    // size of arrays of pointers (for batched cases) and re-usable workspace
    size_t size_work_workArr;
    // extra requirements for calling larf and larfg
    size_t size_Abyx_norms;
    rocsolver_gebd2_getMemorySize<T, false>(m, n, batch_count, &size_scalars, &size_work_workArr,
                                            &size_Abyx_norms);

    if(rocblas_is_device_memory_size_query(handle))
        return rocblas_set_optimal_device_memory_size(handle, size_scalars, size_work_workArr,
                                                      size_Abyx_norms);

    // memory workspace allocation
    void *scalars, *work_workArr, *Abyx_norms;
    rocblas_device_malloc mem(handle, size_scalars, size_work_workArr, size_Abyx_norms);

    if(!mem)
        return rocblas_status_memory_error;

    scalars = mem[0];
    work_workArr = mem[1];
    Abyx_norms = mem[2];
    if(size_scalars > 0)
        init_scalars(handle, (T*)scalars);

    // execution
    return rocsolver_gebd2_template<S, T>(handle, m, n, A, shiftA, lda, strideA, D, strideD, E,
                                          strideE, tauq, strideQ, taup, strideP, batch_count,
                                          (T*)scalars, work_workArr, (T*)Abyx_norms);
}

/*
 * ===========================================================================
 *    C wrapper
 * ===========================================================================
 */

extern "C" {

rocblas_status rocsolver_sgebd2_strided_batched(rocblas_handle handle,
                                                const rocblas_int m,
                                                const rocblas_int n,
                                                float* A,
                                                const rocblas_int lda,
                                                const rocblas_stride strideA,
                                                float* D,
                                                const rocblas_stride strideD,
                                                float* E,
                                                const rocblas_stride strideE,
                                                float* tauq,
                                                const rocblas_stride strideQ,
                                                float* taup,
                                                const rocblas_stride strideP,
                                                const rocblas_int batch_count)
{
    return rocsolver_gebd2_strided_batched_impl<float, float>(handle, m, n, A, lda, strideA, D,
                                                              strideD, E, strideE, tauq, strideQ,
                                                              taup, strideP, batch_count);
}

rocblas_status rocsolver_dgebd2_strided_batched(rocblas_handle handle,
                                                const rocblas_int m,
                                                const rocblas_int n,
                                                double* A,
                                                const rocblas_int lda,
                                                const rocblas_stride strideA,
                                                double* D,
                                                const rocblas_stride strideD,
                                                double* E,
                                                const rocblas_stride strideE,
                                                double* tauq,
                                                const rocblas_stride strideQ,
                                                double* taup,
                                                const rocblas_stride strideP,
                                                const rocblas_int batch_count)
{
    return rocsolver_gebd2_strided_batched_impl<double, double>(handle, m, n, A, lda, strideA, D,
                                                                strideD, E, strideE, tauq, strideQ,
                                                                taup, strideP, batch_count);
}

rocblas_status rocsolver_cgebd2_strided_batched(rocblas_handle handle,
                                                const rocblas_int m,
                                                const rocblas_int n,
                                                rocblas_float_complex* A,
                                                const rocblas_int lda,
                                                const rocblas_stride strideA,
                                                float* D,
                                                const rocblas_stride strideD,
                                                float* E,
                                                const rocblas_stride strideE,
                                                rocblas_float_complex* tauq,
                                                const rocblas_stride strideQ,
                                                rocblas_float_complex* taup,
                                                const rocblas_stride strideP,
                                                const rocblas_int batch_count)
{
    return rocsolver_gebd2_strided_batched_impl<float, rocblas_float_complex>(
        handle, m, n, A, lda, strideA, D, strideD, E, strideE, tauq, strideQ, taup, strideP,
        batch_count);
}

rocblas_status rocsolver_zgebd2_strided_batched(rocblas_handle handle,
                                                const rocblas_int m,
                                                const rocblas_int n,
                                                rocblas_double_complex* A,
                                                const rocblas_int lda,
                                                const rocblas_stride strideA,
                                                double* D,
                                                const rocblas_stride strideD,
                                                double* E,
                                                const rocblas_stride strideE,
                                                rocblas_double_complex* tauq,
                                                const rocblas_stride strideQ,
                                                rocblas_double_complex* taup,
                                                const rocblas_stride strideP,
                                                const rocblas_int batch_count)
{
    return rocsolver_gebd2_strided_batched_impl<double, rocblas_double_complex>(
        handle, m, n, A, lda, strideA, D, strideD, E, strideE, tauq, strideQ, taup, strideP,
        batch_count);
}

} // extern C
