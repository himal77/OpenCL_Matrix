/**
 * This is helper for OpenCL to return Program
 *
 * @Author Himal Puri
 */

#include <CL/cl.hpp>
#include "vector"
#include "string"
#include "fstream"
#include "assert.h"

cl::Program createPlatform(const std::string& file) {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    assert(platforms.size() > 0);

    auto platform = platforms.front();
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

    auto device = devices.front();

    std::ifstream openCLFile(file);
    std::string src(std::istreambuf_iterator<char>(openCLFile), (std::istreambuf_iterator<char>()));

    cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));

    cl::Context context(device);
    cl::Program program(context, sources);

    program.build("-cl-std=CL1.2");
    return program;
}