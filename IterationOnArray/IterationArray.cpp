/**
 * N times iteration on Array
 *
 * @Author Himal Puri
 */


#include <cstring>
#include "CL/cl.h"
#include "fstream"
#include "string"
#include "iostream"

#define SUCCESS 0
#define FAILURE 1
#define NUM_LAPS 4

const std::string &file = "kernel.cl";

int main() {


    /* STEP 1: Getting platforms and choose an available one.*/
    cl_uint numPlatforms;
    cl_platform_id platform = NULL;
    cl_int status = clGetPlatformIDs(0, NULL, &numPlatforms);

    if (status != CL_SUCCESS) {
        std::cout << "Error: Getting platforms!" << std::endl;
        return FAILURE;
    }

    /* Choosing the first available platform */
    if (numPlatforms > 0) {
        cl_platform_id *platforms = (cl_platform_id *) malloc(numPlatforms * sizeof(cl_platform_id));
        status = clGetPlatformIDs(numPlatforms, platforms, NULL);
        platform = platforms[0];
        free(platforms);
    }

    /* STEP 2: query the platform and choose the first GPU device if has one. Otherwise use the CPU as device. */
    cl_uint numDevices = 0;
    cl_device_id *devices;

    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);

    if(numDevices == 0) { // no gpu available
        std::cout<<"No GPU device available. "<<std::endl;
        std::cout<<"Choose CPU as default device." << std::endl;
        status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL, &numDevices);
        devices = (cl_device_id*) malloc(numDevices * sizeof(cl_device_id));
        status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, numDevices, devices, NULL);
    } else {
        devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
        status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
    }


    /* STEP 3: Create Context */
    cl_context context = clCreateContext(NULL, 1, devices, NULL, NULL, NULL);

    /* STEP 4: Creating command queue associate with the context. */
    cl_command_queue queue = clCreateCommandQueue(context, devices[0],  0, NULL);

    /* STEP 5: creating program object from .cl file */
    std::ifstream openCLFile(file);
    std::string src(std::istreambuf_iterator<char>(openCLFile), (std::istreambuf_iterator<char>()));
    cl_program program = clCreateProgramWithSource(context, 1, (const char **) &src, NULL, NULL);

    /* STEP 6: Building the program. */
    status = clBuildProgram(program, 1, devices, NULL, NULL, NULL);

    /* STEP 7: Initialize the input output for the host */
    int input[] = {1, 2, 3, 4, 5};
    size_t length = sizeof(input)/sizeof(*input);
    for(int i = 0; i < length; i++) {
        std::cout << "Input : " << input[i] << std::endl;
    }
    int output[length];

    /* STEP 8: Create memory objects for kernel */
    cl_mem inBuf = clCreateBuffer(context, CL_MEM_READ_WRITE, (length ) * sizeof(int), (void *)input, NULL);
    cl_mem outBuf = clCreateBuffer(context, CL_MEM_READ_WRITE, (length ) * sizeof(int), NULL, NULL);

    /* STEP 9: Create kernel objects*/
    cl_kernel kernel = clCreateKernel(program, "my", NULL);

    /* STEP 10: Sets kernel arguments. */
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &inBuf);
    std::cout<<"Status code of clSetKernelArg(): "<<status<<std::endl;

    /* STEP 11: Running the kernel */
    size_t global_work_size[1] = {length};
    for(int i = 0; i < NUM_LAPS; i++) {
        status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
    }
    std::cout<<"Status code of clEnqueuNDRangeKernel(): "<<status<<std::endl;

    status = clFinish(queue);
    std::cout<<"Status code of clFinish(): "<<status<<std::endl;

    /* STEP 12: Read the output back to host memory. */
    status = clEnqueueReadBuffer(queue, outBuf, CL_TRUE, 0, length * sizeof(char), output, 0, NULL, NULL);
    std::cout<<"Status code of clEnqueueReadBuffer(): "<<status<<std::endl;

    /* STEP 13: Waiting for all command to finish */
    status = clFinish(queue);
    std::cout<<"Status code of clFinish(): "<<status<<std::endl;

    /* STEP 13: Clean the resources. */
    status = clReleaseKernel(kernel);
    status = clReleaseProgram(program);
    status = clReleaseMemObject(inBuf);
    status = clReleaseMemObject(outBuf);
    status = clReleaseCommandQueue(queue);
    status = clReleaseContext(context);

    for(int i = 0; i < length; i++) {
        std::cout << "Output : " << input[i] << std::endl;
    }

    free(devices);
    devices = nullptr;

    std::cout<<"Passed! " << "Success code: " << status<<std::endl;
    return SUCCESS;



}

