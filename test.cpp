
#include <assert.h>
#include <CL/cl.hpp>
#include "iostream"


#include <assert.h>
#include <CL/cl.hpp>
#include "iostream"


int main(void) {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    assert(platforms.size() > 0);

    auto platform = platforms.front();
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);


    assert(devices.size() > 0);

    auto device = devices.front();
    auto vendor = device.getInfo<CL_DEVICE_VENDOR>();
    auto version = device.getInfo<CL_DEVICE_VERSION>();

    std::cout << vendor;
    std::cout << version;


}
