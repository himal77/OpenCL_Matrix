//
// Created by puri on 1/23/21.
//

#ifndef MATRIX_OPENCL_HELPER_H
#define MATRIX_OPENCL_HELPER_H

#include "CL/cl.hpp"

/*
 * Method for creating platform for all the program
 */
cl::Program createPlatform(const std::string& file);

#endif //MATRIX_OPENCL_HELPER_H