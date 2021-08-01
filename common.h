#ifndef __common_h__
#define __common_h__

#include <cstring>
#include <string>
#include <fstream>
#include <iostream>

namespace CommonCLI {
    //Maybe use std optionals here for c++17, instead of returning defaults and -1, but CUDA seems to error when spec'd above c++14.
    int find_arg_idx(int argc, char** argv, const char* option);
    int find_int_arg(int argc, char** argv, const char* option, int default_value);
    unsigned long long find_ull_arg(int argc, char** argv, const char* option, int default_value);
    float find_float_arg(int argc, char** argv, const char* option, float default_value);
    char* find_string_option(int argc, char** argv, const char* option, char* default_value);
}
#endif
