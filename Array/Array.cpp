/**
 * Array with OpenCL
 *
 * @Author Himal Puri
 */


#include <CL/cl.hpp>
#include "iostream"
#include "../Helper.h"
#include "algorithm"

int main() {
    cl::Program program = createPlatform("Array.cl");
    auto context = program.getInfo<CL_PROGRAM_CONTEXT>();
    auto devices = context.getInfo<CL_CONTEXT_DEVICES>();
    auto &device = devices.front();

    std::vector<int> vec(1024);

    cl_int err = 0;
    cl::Buffer inBuf(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_ALLOC_HOST_PTR,
                     sizeof(int) * vec.size(), vec.data(), &err);
    cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY,
                      sizeof(int) * vec.size(), &err);

    // declaring the kernel with its method name
    cl::Kernel kernel(program, "ProcessArray");

    // setting argument for the kernel methods
    err = kernel.setArg(0, inBuf);
    err = kernel.setArg(1, outBuf);

    // creating queue command for executing on kernel
    cl::CommandQueue queue(context, device);

    // Filling the vector
    err = queue.enqueueFillBuffer(inBuf, 3, sizeof(int) * 10, sizeof(int) * (vec.size() - 10));

    // Calling the kernel with range operation
    err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(vec.size()));

    // reading the data from the kernel
    err = queue.enqueueReadBuffer(outBuf, CL_FALSE, 0, sizeof(int) * vec.size(), vec.data());

    // waiting for kernel to finish
    cl::finish();

    std::cout<<"Element of vector is: ";

    std::for_each(vec.begin(), vec.end(), [](int& a){
        std::cout<<a<<std::endl;
    });

}