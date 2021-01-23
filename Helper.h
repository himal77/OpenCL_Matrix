#ifndef HELPER_H
#define HELPER_H

#include "CL/cl.hpp"

/*
 * Method for creating platform for all the program
 */
cl::Program createPlatform(const std::string& file);

#endif