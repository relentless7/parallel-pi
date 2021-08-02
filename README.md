# pi-trials

This is just a light framework to provide a basis for testing some parallel programming problems.

Here we are using the classic monte carlo pi approximation where we throw a dart at a square with a circle embedded in it.
The ratio of square to circle is 4/Pi, so by throwing a large number of darts we should approximate that ratio. 

This program demonstrates heterogenous computing by running the same kernel on both the cpu and gpu concurrently.

The goal of this is to just have a nice little example of how to integrate C++, CUDA, and OpenMP together and have it generate to 
your environment of choice via CMAKE so we can build and test it quickly in many environments. 

Using different hardware and different compilers can really expose your solution and help write stronger ones!


## Dependencies

cmake 3.21 - https://cmake.org/download/  
** using pip to manage the latest cmake is easiest

cuda - https://developer.nvidia.com/cuda-downloads 
** installing AFTER Visual Studio is already installed will fix up a lot of environment variables.


## Build Instructions

```
mkdir build  
cd build  
cmake CMAKE_BUILD_TYPE=Release ..  
```
You can now run your generator, I prefer "ninja", but "make" is usually default for Linux and "msbuild.exe" for Windows. 
Missing environment variables is usually a sign of a bad install as most of them get set automatically.
  
Here is an example of building for Windows from commandline:

`"\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" pi_trials.sln -m /property:Configuration=Release`

## Run Instructions
`./pi-trials -h for help`

example:

"Throw 1 billion darts, with 12 threads on the CPU(s)."  
`./pi-trials -n 1000000000 -p 12 -t 2`

"Throw 1 billion darts, using a GPU."  
`./pi-trials -n 1000000000 -t 3`

"Throw 1 billion darts, with the GPU doing 770,000,000 and CPU doing 30,000,000 trials over 12 threads."  
`./pi-trials -n 1000000000 -p 12 -t 4 -r 0.97`

## PI Methods

1. Approx. using the Integral from 0 to 1 of 4.0/(1+x^2) dx = Pi
2. Monte Carlo Darts - CPU
3. Monte Carlo Darts - GPU
4. Monte Carlo Darts - GPU + CPU


## Comments

The GPU is flat out faster than the CPU. Offloading a portion from the GPU that the CPU can finish before the GPU usually* yields a slight improvement in total
processing time. Also as you will notice that there is some overhead to using CUDA at all, so the CPU still wins out when the number of problems it has to solve
is smaller than that threshold. Things get more interesting with more computationally complex problems and irregular computation. Very custom and tuned solutions
are necessary for raw performance.  

Every platform is also different and might need tuning at the compiler level and the libraries themselves might be implemented differently. So... YMMV.

This code was meant to serve as a nice starting point with an emphasis on CUDA and doing as much as possible on the GPU.  

Hopefully this example inspires you to experiment with CUDA and the build framework makes this easy for you to get your own projects rolling!  

