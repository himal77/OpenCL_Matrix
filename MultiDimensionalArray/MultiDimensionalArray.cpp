/**
 * MultiDimensional Array with OpenCL
 *
 * @Author Himal Puri
 */


#include <CL/cl.hpp>
#include "iostream"
#include "../Helper.h"
#include "algorithm"
#include "array"

int main() {
    cl::Program program = createPlatform("MultiDimensionalArray.cl");
    auto context = program.getInfo<CL_PROGRAM_CONTEXT>();
    auto devices = context.getInfo<CL_CONTEXT_DEVICES>();
    auto &device = devices.front();

    // Initializing array
    const int row = 3;
    const int col = 2;
    const int noOfElem = row * col;
    std::array<std::array<int, col>, row> arr = {{{1, 1}, {2, 2}, {3, 3}}};

    // Buffer for the Array
    cl::Buffer buf(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * noOfElem,
                   arr.data());

    // Creating the kernel for the operation
    cl::Kernel kernel(program, "ProcessMultiDimensionalArray");

    // Setting the arguments of kernel
    kernel.setArg(0, buf);

    // CommandQueue to execute command
    cl::CommandQueue queue(context, device);

    // Execute the command
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(2, 3));

    // Read the command back in
    queue.enqueueReadBuffer(buf, CL_TRUE, 0, sizeof(int) * noOfElem, arr.data());

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            std::cout << "VAL: " << arr[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cin.get();

}