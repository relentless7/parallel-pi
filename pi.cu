#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#include <curand.h>
#include <time.h>
#include "pi.h"
#include <math.h>
#include <random>

#include <thrust/scan.h>
#include <thrust/execution_policy.h>

//Kernel taken from  OakRidge Labs, what it does it pretty straight forward...
__global__ void throwdarts_kernel(int* hits, float* randomnums)
{
	int i;
	double x,y,z;
	int tid = blockDim.x * blockIdx.x + threadIdx.x;
	i = tid;
	int xidx = 0, yidx = 0;

	xidx = (i+i);
	yidx = (xidx+1);

	x = randomnums[xidx];
	y = randomnums[yidx];
	z = ((x*x)+(y*y));

	if (z<=1)
		hits[tid] = 1;
	else
		hits[tid] = 0;	
}

void CUDAErrorCheck()
{
	cudaError_t error = cudaGetLastError();
	if (error != cudaSuccess)
	{	
		printf("CUDA error : %s (%d)\n", cudaGetErrorString(error), error);
		exit(0);
	}
}

#define NUM_THREADS 1024

//wrapper for CUDA calls
std::int64_t ThrowDarts(std::int64_t num_trials)
{
	int device_num;
	size_t mem_free = 0;
	size_t mem_tot = 0;
	
	cudaSetDevice(0);
	cudaGetDevice(&device_num);
	cudaDeviceReset();
	cudaMemGetInfo  (&mem_free, &mem_tot);
	//std::cout<<"Free memory before copy dev: "<<mem_free<<" Total: "<<  mem_tot <<std::endl;	

	std::int64_t num_trials_remaining = num_trials; 
	float *randomnums;
		
	std::int64_t max_mem_per_iter = mem_free - 30000000; //magic number to leave enough buffer so we don't run out of memory.
	std::int64_t max_trials_per_iter = max_mem_per_iter/(2 * sizeof(float) + sizeof(int));	

	//we have to promote up to 64-bit math to do this roundup(uint/uint) = uint + uint - 1/uint is not safe at max values
	//instead double holds a uint max size variable, we will use ceil and round up.
	std::int64_t number_of_iters = ceil((double)num_trials_remaining/(double)max_trials_per_iter);

	//std::cout << "Max Trials per iteration: " << max_trials_per_iter << std::endl;	
	//std::cout << "Number of kernel launches needed: " << number_of_iters << std::endl;
	std::int64_t reducedcount = 0;

	int blocks = (max_trials_per_iter + NUM_THREADS - 1) / NUM_THREADS;
	int threads = NUM_THREADS;

	cudaMalloc((void**)&randomnums, (2 * max_trials_per_iter) * sizeof(float));
	CUDAErrorCheck();

	int* device_hits;
	int hit_count;

	cudaMalloc((void**)&device_hits, max_trials_per_iter * sizeof(int));
	CUDAErrorCheck();

	while(num_trials_remaining > 0)
	{
		std::int64_t num_trials_to_launch = min(num_trials_remaining, max_trials_per_iter);
		num_trials_remaining -= num_trials_to_launch;
	
		// Use CuRand to generate an array of random numbers on the device
		int status;
		curandGenerator_t gen;
		status = curandCreateGenerator(&gen, CURAND_RNG_PSEUDO_MRG32K3A);
		status |= curandSetPseudoRandomGeneratorSeed(gen, 4294967296ULL^time(NULL));
		status |= curandGenerateUniform(gen, randomnums, (2*num_trials_to_launch));
		status |= curandDestroyGenerator(gen);
		if (status != CURAND_STATUS_SUCCESS)
		{
			printf("CuRand Failure\n");
			exit(EXIT_FAILURE);
		}

		blocks = (num_trials_to_launch + threads - 1) / threads;	

		//one dart throw per thread
		throwdarts_kernel <<<blocks, threads>>> (device_hits, randomnums);
		cudaDeviceSynchronize();
		CUDAErrorCheck();
		
		//leave the results on the device and do an inclusive scan (rather than count them on the host)
		thrust::inclusive_scan(thrust::device, device_hits, device_hits + num_trials_to_launch, device_hits);
		//mem copy the last element, it contains the total hit count
		cudaMemcpy(&hit_count, device_hits + num_trials_to_launch-1, sizeof(int), cudaMemcpyDeviceToHost);
		
		reducedcount = reducedcount + hit_count;

	}

	cudaFree(randomnums);
	cudaFree(device_hits);

	return reducedcount; 
}

double GPUMonteCarloPi::CalculateApproximation(std::int64_t num_trials)
{
	return ((double)ThrowDarts(num_trials)/(double)num_trials) * 4.0;
}

double HybridMonteCarloPi::CalculateApproximation(std::int64_t num_trials)
{
	std::int64_t gpu_hits = 0;
	std::int64_t cpu_hits = 0;
	std::int64_t num_gpu_trials = (std::int64_t)(num_trials * gpu_ratio);
	std::int64_t num_cpu_trials = num_trials - num_gpu_trials;

    #pragma omp parallel
    {
		#pragma omp single nowait
		{
			gpu_hits = ThrowDarts(num_gpu_trials);
		}
				
		//this blob is not a a static function because getting cpu_hits to remain public in that context
		//is too difficult, so we will deal with a tiny bit of bloat.
		std::random_device rseed;
		std::mt19937 rgen(rseed()); // mersenne_twister
		std::uniform_real_distribution<double> rdist(0.0,1.0);
					
		#pragma omp for schedule(guided) reduction(+:cpu_hits)
		for(std::int64_t i=0; i < num_cpu_trials; ++i)
		{
			double random_x = rdist(rgen);
			double random_y = rdist(rgen);
			//if we are in the circle increment the count.
			if(random_x*random_x + random_y*random_y <= 1.0)
			{
				cpu_hits = cpu_hits + 1; 
			}
		}
    }
	//largest double can actually hold the largest int by standard
	return (double)(cpu_hits + gpu_hits)/(double)num_trials * 4.0;
}
