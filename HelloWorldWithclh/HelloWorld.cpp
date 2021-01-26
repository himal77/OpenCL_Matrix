/**
 * This is just simple hello world using cl.h file instead of cl.hpp
 *
 * @Author: Himal Puri
 */

#include <cstring>
#include "CL/cl.h"
#include "fstream"
#include "string"
#include "iostream"

#define SUCCESS 0
#define FAILURE 1
const std::string &file = "kernel.cl";

int main() {

    /**
     * Setting up the OpenCL
     * Creating device, context and queue
     */

    /* Step1: Getting platforms and choose an available one.*/
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

    /* Step 2: query the platform and choose the first GPU device if has one. Otherwise use the CPU as device. */
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


    /* Step 3: Create Context */
    cl_context context = clCreateContext(NULL, 1, devices, NULL, NULL, NULL);

    /* Step 4: Creating command queue associate with the context. */
    cl_command_queue queue = clCreateCommandQueue(context, devices[0],  0, NULL);

    /* STEP 5: creating program object from .cl file */
    std::ifstream openCLFile(file);
    std::string src(std::istreambuf_iterator<char>(openCLFile), (std::istreambuf_iterator<char>()));
    cl_program program = clCreateProgramWithSource(context, 1, (const char **) &src, NULL, NULL);

    /* STEP 6: Building the program. */
    status = clBuildProgram(program, 1, devices, NULL, NULL, NULL);

    /* STEP 7: Initialize the input output for the host and create memory objects for the kernel */
    const char* input = "ABCDEF";
    size_t strLength = std::strlen(input);
    std::cout<<"Input String: "<<input<<std::endl;
    char *output = (char*) malloc(strLength + 1);

    cl_mem inBuf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, (strLength + 1) * sizeof(char), (void *)input, NULL);
    cl_mem outBuf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, (strLength + 1) * sizeof(char), NULL, NULL);

    /* STEP 8: Create kernel objects*/
    cl_kernel kernel = clCreateKernel(program, "helloworld", NULL);

    /* STEP 9: Sets kernel arguments. */
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &inBuf);
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &outBuf);

    /* STEP 10: Running the kernel */
    size_t global_work_size[1] = {strLength};
    status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);

    /* STEP 11: Read the cout put back to host memory. */
    status = clEnqueueReadBuffer(queue, outBuf, CL_TRUE, 0, strLength * sizeof(char), output, 0, NULL, NULL);

    output[strLength] = '\0';
    std::cout<<"\noutput string: "<<std::endl;
    std::cout<<output<<std::endl;

    /* STEP 12: Clean the resources. */
    status = clReleaseKernel(kernel);
    status = clReleaseProgram(program);
    status = clReleaseMemObject(inBuf);
    status = clReleaseMemObject(outBuf);
    status = clReleaseCommandQueue(queue);
    status = clReleaseContext(context);

    if(output != NULL) {
        free(output);
        output = NULL;
    }

    if( devices != NULL) {
        free(devices);
        devices = NULL;
    }

    std::cout<<"Passed! \n";
    return SUCCESS;



}


