/**
 * Matrix addition with OpenCL
 *
 * @Author Himal Pur
 */


#include "CL/opencl.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>

const std::string &file = "kernel.cl";

/**
 * Function for initializing value of A & B
 *
 * @param A Matrix
 * @param n width
 * @param m height
 * @param c value
 */
void initMatrix(float *A, int n, int m, float c) {
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++)
            A[i * m + j] = c;
}


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
    const int N = 4, M = 4;
    float *A = (float *) malloc(N * M * sizeof(float));
    float *B = (float *) malloc(N * M * sizeof(float));
    float *C = (float *) malloc(N * M * sizeof(float));
    initMatrix((float *) A, N, M, 1.0);
    initMatrix((float *) B, N, M, 2.0);
    initMatrix((float *) C, N, M, 0.0);


    /**
     * Allocating buffer for three matrix.
     */
    cl_mem A_dev = clCreateBuffer(context, CL_MEM_READ_ONLY, N * M * sizeof(float), A, NULL);
    cl_mem B_dev = clCreateBuffer(context, CL_MEM_READ_ONLY, N * M * sizeof(float), B, NULL);
    cl_mem C_dev = clCreateBuffer(context, CL_MEM_READ_WRITE, N * M * sizeof(float), C, NULL);

    /**
     * Write the matrix A & B to CommandQueue
     */
    err = clEnqueueWriteBuffer(queue, A_dev, CL_TRUE, 0, N * M * sizeof(float), A, 0, NULL, NULL);
    err = clEnqueueWriteBuffer(queue, B_dev, CL_TRUE, 0, N * M * sizeof(float), B, 0, NULL, NULL);
    std::cout<<"err at clEnqueueWriteBuffer(): "<<err<<std::endl;

    /**
     * Creating the string from source file
     * Creating program with source file
     * Build the program
     */
    std::ifstream openCLFile(file);
    std::string src(std::istreambuf_iterator<char>(openCLFile), (std::istreambuf_iterator<char>()));
    cl_program program = clCreateProgramWithSource(context, 1, (const char **) &src, NULL, NULL);

    err = clBuildProgram(program, 0, NULL, "", NULL, NULL);
    std::cout<<"err at clBuildProgram(): "<<err<<std::endl;

    cl_kernel kernel = clCreateKernel(program, "gpuMatAdd", &err);


    /**
     * Setting the kernel arguments
     */
    unsigned int KernelArgIndices = 0;
    err |= clSetKernelArg(kernel, KernelArgIndices++, sizeof(cl_mem), &A_dev);
    err |= clSetKernelArg(kernel, KernelArgIndices++, sizeof(cl_mem), &B_dev);
    err |= clSetKernelArg(kernel, KernelArgIndices++, sizeof(cl_mem), &C_dev);
    err |= clSetKernelArg(kernel, KernelArgIndices++, sizeof(int), &N);
    err |= clSetKernelArg(kernel, KernelArgIndices++, sizeof(int), &M);
    std::cout<<"err at clSetKernelArg(): "<<err<<std::endl;


    /**
     * Creating global and local work size
     *
     * @workgroup global size
     * @ndrange global work offset
     */
    size_t local_workgroup[2] = {2, 2};
    size_t global_ndrange[2] = {N, M};
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_ndrange, local_workgroup, 0, NULL, NULL);
    std::cout<<"err at clEnqueueNDRangeKernel(): "<<err<<std::endl;

    /* Reading from the queue for C */
    err = clEnqueueReadBuffer(queue, C_dev, CL_TRUE, 0, N * M * sizeof(float), C, 0, NULL, NULL);
    std::cout<<"err at clEnqueueReadBuffer(): "<<err<<std::endl;
    std::cout << C[0] << "," << C[1] << "," << C[3] << "\n";

    clReleaseMemObject(A_dev);
    clReleaseMemObject(B_dev);
    clReleaseMemObject(C_dev);
    free(A);
    free(B);
    free(C);

    return 0;
}