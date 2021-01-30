/**
 * Matrix addition with OpenCL
 *
 * @Author Himal Puri
 */


#include "CL/opencl.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <iomanip>
#include "random"
#include "chrono"

#define MATRIX_SIZE 8
#define ITERATION 1
#define KERNEL_FILE "kernel.cl"


#define precision 2

void getErrorCode(std::string from, int errCode) {
    std::cout << from << errCode << std::endl;
}

int main(int argc, char *argv[]) {

    /**
     * Initializing 2 matrix A, B.
     */
    const size_t NUM_ITERATION = ITERATION;
    const size_t N = MATRIX_SIZE;
    float *A = (float *) malloc(N * N * sizeof(float));
    float *B = (float *) malloc(N * N * sizeof(float));


    /**
     * A is C & D is B w.r.t value.
     * A and B will be parallel.
     * C and D will be serial version to compare
     */
    float *C = (float *) malloc(N * N * sizeof(float));
    float *D = (float *) malloc(N * N * sizeof(float));

    /*
     * Declearing the matrix for local in kernel.
     */
    const size_t localSize = 2;
    float *localA = (float *) malloc(localSize * localSize * sizeof(float));
    float *localB = (float *) malloc(localSize * localSize * sizeof(float));

    /**
     * @if size is four.
     * This is for checking purpose given in assignment instruction.
     *
     * @else
     * initialize with random number less than 100.
     *
     */
    if (N == 4) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if ((i * N + j) == 6) {
                    A[i * N + j] = C[i * N + j] = 10.0f;
                } else if ((i * N + j) == 4) {
                    A[i * N + j] = C[i * N + j] = 5.0f;
                } else if ((i * N + j) == 12) {
                    A[i * N + j] = C[i * N + j] = 15.0f;
                } else {
                    A[i * N + j] = C[i * N + j] = 0.0f;
                }
            }
        }
    } else {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i * N + j] = C[i * N + j] = rand() % 100;
            }
        }
    }


    /**
     * Printing A for testing
     */
    std::cout << " Original A version: \n";
    std::cout << std::setprecision(precision) << std::fixed;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << A[N * i + j] << " ";
        }
        std::cout << "\n";
    }


    /////////////////////////////////////////////////
    ///////////////Serial Version////////////////////
    /////////////////////////////////////////////////

    /**
    *  inserting into matrix for serial version
    *  @iteration: no of times the matrix will be iterated
    *
    *  @A: matrix which will store the data
    *  @B: helper matrix for smoothing out the iteration
    */
    auto start = std::chrono::high_resolution_clock::now();
    for (int iteration = 0; iteration < NUM_ITERATION; iteration++) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {

                /* Index of next item */
                int index = i * N + j;

                float left = (index % N) == 0 ? 0.0 : C[index - 1];
                float top = (index - N) < 0 ? 0.0 : C[index - N];
                float right = (index % N) == N - 1 ? 0.0 : C[index + 1];
                float bottom = (index + N) >= N * N ? 0.0 : C[index + N];

                // Adding new value to new A using B
                D[i * N + j] = 0.2 * (
                        C[index] +
                        left +
                        right +
                        top +
                        bottom
                );
            }
        }


        /**
         * Assigning value of A to B for next iteration
         * if its last iteration, no need to assign
         */
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                C[i * N + j] = D[i * N + j];
            }
        }

    }
    auto end = std::chrono::high_resolution_clock::now();
    auto serialTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();



    /////////////////////////////////////////////////
    ///////////////Parallel Version//////////////////
    /////////////////////////////////////////////////

    /**
    *  Creating device from available platform
    *  Initializing context and command queue
    */
    cl_platform_id platformID;
    cl_device_id device_id;
    cl_int err = clGetPlatformIDs(1, &platformID, NULL);
    err = clGetDeviceIDs(platformID, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);

    /**
     * Creating context and command queue for device.
     */
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    cl_command_queue queue = clCreateCommandQueue(context, device_id, 0, &err);

    /**
     * Allocating buffer for four matrix.
     */


    cl_mem A_dev = clCreateBuffer(context, CL_MEM_READ_ONLY, N * N * sizeof(float), A, NULL);
    cl_mem B_dev = clCreateBuffer(context, CL_MEM_READ_ONLY, N * N * sizeof(float), B, NULL);

    /**
     * Write the matrix A to CommandQueue
     */
    err = clEnqueueWriteBuffer(queue, A_dev, CL_TRUE, 0, N * N * sizeof(float), A, 0, NULL, NULL);
    getErrorCode("err at clEnqueueWriteBuffer(): ", err);


    /**
     * Creating the string from source file.
     * Creating program with source file.
     * Build the program.
     * Checking error if exist.
     */
    std::ifstream openCLFile(KERNEL_FILE);
    std::string src(std::istreambuf_iterator<char>(openCLFile), (std::istreambuf_iterator<char>()));
    cl_program program = clCreateProgramWithSource(context, 1, (const char **) &src, NULL, NULL);
    err = clBuildProgram(program, 0, NULL, "-cl-single-precision-constant", NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "gpuMatAdd", &err);
    getErrorCode("err at clBuildProgram(): ", err);


    /**
     * Setting the kernel arguments
     */



    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &A_dev);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &B_dev);
    err = clSetKernelArg(kernel, 2, sizeof(int), &N);
    err = clSetKernelArg(kernel, 3, sizeof(int), &NUM_ITERATION);
    err = clSetKernelArg(kernel, 4, localSize * localSize * sizeof(cl_float), NULL);
    err = clSetKernelArg(kernel, 5, localSize * localSize * sizeof(cl_float), NULL);
    getErrorCode("err at clSetKernelArg(): ", err);

    /**
     * Creating global and local work size
     *
     * @workgroup global size
     * @ndrange global work offset
     */
    size_t localWorkSize[2];
    if (N <= 32) {
        localWorkSize[0] = localSize;
        localWorkSize[1] = localSize;
    } else {
        localWorkSize[0] = localSize;
        localWorkSize[1] = localSize;
    }
    size_t globalWorkSize[2] = {N, N};

    /**
     * TODO optimize this loop
     * Since data movement from host to device is slow, this program is slow
     *
     * How to use this function in kernel?
     *
     * for(int i = 0; i < NUM_ITERATION, i++){
     *  Execute the computation here for A
     *  Put in matrix B
     *
     *  global Barrier here / local barrier
     *
     *  Put back B  to A.
     *
     *  global barrier here / local barrier
     * }
     */
    start = std::chrono::high_resolution_clock::now();

    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
    getErrorCode("err at clEnqueueNDRangeKernel(): ", err);

    /* Reading from the queue for B */
    err = clEnqueueReadBuffer(queue, B_dev, CL_TRUE, 0, N * N * sizeof(float), B, 0, NULL, NULL);
    getErrorCode("err at clEnqueueReadBuffer(): ", err);

    clFinish(queue);

    end = std::chrono::high_resolution_clock::now();
    auto parallelTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();


    /////////////////////////////////////////////////
    //Matrix Display if size is 32 only Otherwise it will be pretty big////////////////////
    /////////////////////////////////////////////////


    if (N < 9) {
        /**
         * displaying matrix in serial version
         * As well as the time it takes to compute
         */
        std::cout << "Serial version: \n";
        std::cout << std::setprecision(precision) << std::fixed;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                std::cout << C[i * N + j] << " ";
            }
            std::cout << "\n";
        }


        /**
         * displaying matrix in serial version
         * As well as the time it takes to compute
         */
        std::cout << "Parallel version: \n";
        std::cout << std::setprecision(precision) << std::fixed;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                std::cout << B[N * i + j] << " ";
            }
            std::cout << "\n";
        }
    }

    std::cout << "Time taken by Serial version: " << serialTime << " microseconds" << std::endl;
    std::cout << "Time taken by Parallel version: " << parallelTime << " microseconds" << std::endl;

    clReleaseMemObject(A_dev);
    clReleaseMemObject(B_dev);

    free(A);
    free(B);


    return 0;
}