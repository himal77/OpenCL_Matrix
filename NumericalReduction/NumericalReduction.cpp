/**
 * NumericalReduction Array with OpenCL
 *
 * @Author Himal Puri
 */


#include <CL/cl.hpp>
#include "iostream"
#include "../Helper.h"
#include "array"

int main() {
    cl::Program program = createPlatform("NumericalReduction.cl");
    auto context = program.getInfo<CL_PROGRAM_CONTEXT>();
    auto devices = context.getInfo<CL_CONTEXT_DEVICES>();
    auto &device = devices.front();

    std::vector<int> vec(1024);

    for (int i = 0; i < vec.size(); i++) {
        vec[i] = i;
    }

    cl::Kernel kernel(program, "NumericalReduction");
    cl_int err = 0;
    auto workGroupSize = kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(device, &err);

    auto noOfGroup = vec.size() / workGroupSize;

    cl::Buffer buf(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int) * vec.size(), vec.data());
    cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(int) * noOfGroup);

    kernel.setArg(0, buf);
    kernel.setArg(1, sizeof(int) * noOfGroup, nullptr);
    kernel.setArg(2, outBuf);

    std::vector<int> outVec(noOfGroup);

    cl::CommandQueue queue(context, device);
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(vec.size()), cl::NDRange(workGroupSize));
    queue.enqueueReadBuffer(outBuf, CL_TRUE, 0, sizeof(int) * outVec.size(), outVec.data());


}