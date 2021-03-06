#ifndef __pi_h__
#define __pi_h__

#include <cstring>
#include <iostream>
#include <cmath>
#include <random>
#include <math.h>
#include "omp.h"
#include <string>

// defining it makes it a little more portable.
# define ACTUAL_PI		3.14159265358979323846	

class ParallelPi
{
    public:
        ParallelPi() { }
        virtual double CalculateApproximation(std::int64_t num_trials) = 0;
        virtual std::string ID() = 0;
        static double CalculateError(double approx_pi){ return fabs(ACTUAL_PI - approx_pi);}
    private:
        std::int64_t num_trials;
};

class MonteCarloPi : public ParallelPi
{
    public:
        MonteCarloPi(){};
        double CalculateApproximation(std::int64_t num_trials);        
        std::string ID() { return "[CPU Darts]";}
};

class IntegralPi : public ParallelPi
{
    public:
        IntegralPi(){};
        double CalculateApproximation(std::int64_t num_trials);
        std::string ID() { return "[Integral]";}
};

class BuffonsNeedlePi : public ParallelPi
{
    public:
        BuffonsNeedlePi(){};
        double CalculateApproximation(std::int64_t num_trials);
        std::string ID() { return "[Buffon's Needle]";}

};

class GPUMonteCarloPi : public ParallelPi
{
    public:
        GPUMonteCarloPi(){};
        double CalculateApproximation(std::int64_t num_trials);
        std::string ID() { return "[GPU Darts]";}
    
};

class HybridMonteCarloPi : public ParallelPi
{
    public:
        HybridMonteCarloPi(double gpu_ratio) : gpu_ratio(gpu_ratio){};
        double CalculateApproximation(std::int64_t num_trials);
        std::string ID() { return "[CPU+GPU Darts @ r= "+ std::to_string(gpu_ratio)+" ]";}
    private:
        double gpu_ratio;
};

#endif