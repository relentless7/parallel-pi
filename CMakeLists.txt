cmake_minimum_required(VERSION 3.21)
project(pi_trials)
enable_language(CUDA)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

find_package(OpenMP)
find_package(CUDAToolkit)

set_source_files_properties(${sources} PROPERTIES
    LANGUAGE CUDA
    LINKER_LANGUAGE CUDA)

add_executable(pi_trials main.cpp pi.cpp pi.cu common.cpp)
target_compile_features(pi_trials PRIVATE cxx_std_14)

target_compile_options(pi_trials PUBLIC
    $<$<COMPILE_LANGUAGE:CUDA>:-gencode=arch=compute_70,code=sm_70>
)
target_compile_options(pi_trials PUBLIC $<$<COMPILE_LANGUAGE:CUDA>:-Xcompiler -fopenmp>)


#add openMP and cuRand
if(OpenMP_CXX_FOUND)
    target_link_libraries(pi_trials PUBLIC OpenMP::OpenMP_CXX ${CUDA_curand_LIBRARY})
endif()

