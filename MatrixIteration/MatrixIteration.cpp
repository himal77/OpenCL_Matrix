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

#define NUM_ITERATION 3
#define precision 2

const std::string &file = "kernel.cl";

int main(int argc, char *argv[]) {

    cl_int err = CL_SUCCESS;
    cl_platform_id platform = NULL;
    cl_platform_id platformID;
    cl_device_id device_id;
    cl_context context;
    cl_command_queue queue;

    /**
     *  Creating device from available platform
     *  Initializing context and command queue
     */
    err = clGetPlatformIDs(1, &platformID, NULL);
    err = clGetDeviceIDs(platformID, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    queue = clCreateCommandQueue(context, device_id, 0, &err);


    /**
     * Initializing 3 matrix A, B, C.
     * @result from OpenCL C = A+B using OpenCL
     */
    const int N = 4;
    float *A = (float *) malloc(N * N * sizeof(float));
    float *B = (float *) malloc(N * N * sizeof(float));

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if ((i * N + j) == 6) {
                A[i * N + j] = 10.0f;
            } else {
                A[i * N + j] = 0.0f;
            }
        }
    }

    /**
     * Displaying matrix A
     */
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << A[N * i + j] << " ";
        }
        std::cout << "\n";
    }

    /**
     * Allocating buffer for three matrix.
     */
    cl_mem A_dev = clCreateBuffer(context, CL_MEM_READ_ONLY, N * N * sizeof(float), A, NULL);
    cl_mem B_dev = clCreateBuffer(context, CL_MEM_READ_ONLY, N * N * sizeof(float), B, NULL);

    /**
     * Write the matrix A to CommandQueue
     */
    err = clEnqueueWriteBuffer(queue, A_dev, CL_TRUE, 0, N * N * sizeof(float), A, 0, NULL, NULL);
    std::cout << "err at clEnqueueWriteBuffer(): " << err << std::endl;

    /**
     * Creating the string from source file
     * Creating program with source file
     * Build the program
     */
    std::ifstream openCLFile(file);
    std::string src(std::istreambuf_iterator<char>(openCLFile), (std::istreambuf_iterator<char>()));
    cl_program program = clCreateProgramWithSource(context, 1, (const char **) &src, NULL, NULL);

    err = clBuildProgram(program, 0, NULL, "-cl-single-precision-constant", NULL, NULL);
    std::cout << "err at clBuildProgram(): " << err << std::endl;

    cl_kernel kernel = clCreateKernel(program, "gpuMatAdd", &err);


    /**
     * Setting the kernel arguments
     */
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &A_dev);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &B_dev);
    err = clSetKernelArg(kernel, 2, sizeof(int), &N);
    std::cout << "err at clSetKernelArg(): " << err << std::endl;


    /**
     * Creating global and local work size
     *
     * @workgroup global size
     * @ndrange global work offset
     */
    size_t localWorkSize[2] = {2, 2};
    size_t globalWorkSize[2] = {N, N};

    std::cout<<"********************We are inside loop now*****************+ \n";
    for(int i = 0; i < NUM_ITERATION; i++) {
        err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
        std::cout << "err at clEnqueueNDRangeKernel(): " << err << std::endl;

        /* Reading from the queue for B */
        err = clEnqueueReadBuffer(queue, B_dev, CL_TRUE, 0, N * N * sizeof(float), B, 0, NULL, NULL);
        std::cout << "err at clEnqueueReadBuffer(): " << err << std::endl;

        clFinish(queue);
        clFlush(queue);


        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i * N + j] =  B[i * N + j];
            }
        }

        err = clEnqueueWriteBuffer(queue, A_dev, CL_TRUE, 0, N * N * sizeof(float), A, 0, NULL, NULL);
        std::cout << "err at clEnqueueWriteBuffer(): " << err << std::endl;
    }
    /**
    * Displaying matrix B
    */
    std::cout << std::setprecision(precision) << std::fixed;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << B[N * i + j] << " ";
        }
        std::cout << "\n";
    }

    clReleaseMemObject(A_dev);
    clReleaseMemObject(B_dev);

    free(A);
    free(B);


    return 0;
}