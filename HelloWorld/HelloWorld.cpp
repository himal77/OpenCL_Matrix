#include <assert.h>
#include <CL/cl.hpp>
#include "iostream"
#include "fstream"
#include "string"
#include "../Helper.h"


int main(void) {

    cl::Program program = createPlatform("HelloWorld.cl");
    auto context = program.getInfo<CL_PROGRAM_CONTEXT>();
    auto devices = context.getInfo<CL_CONTEXT_DEVICES>();
    auto& device = devices.front();

    char buf[16];
    cl::Buffer memBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(buf));
    cl::Kernel kernel(program, "HelloWorld");
    kernel.setArg(0, memBuf);

    cl::CommandQueue queue(context, device);
    queue.enqueueTask(kernel);
    queue.enqueueReadBuffer(memBuf, true, 0, sizeof(buf), buf);

    std::cout<<buf;
    std::cin.get();

}
