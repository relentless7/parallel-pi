#ifndef __pi_h__
#define __pi_h__

#include <cstring>
#include <iostream>
#include <cmath>
#include <random>
#include <math.h>
#include <string>
#include "omp.h"


// defining it makes it a little more portable.
# define ACTUAL_PI		3.14159265358979323846	

class ParallelPi
{
    public:
        ParallelPi() { }
        virtual double CalculateApproximation(unsigned int num_trials) = 0;
        virtual std::string ID() = 0;
        static double CalculateError(double approx_pi){ return fabs(ACTUAL_PI - approx_pi);}
    private:
        unsigned int num_trials;
};

class MonteCarloPi : public ParallelPi
{
    public:
        MonteCarloPi(){};
        double CalculateApproximation(unsigned int num_trials);        
        std::string ID() { return "[CPU Darts]";}
};

class IntegralPi : public ParallelPi
{
    public:
        IntegralPi(){};
        double CalculateApproximation(unsigned int num_trials);
        std::string ID() { return "[Integral]";}
};

class BuffonsNeedlePi : public ParallelPi
{
    public:
        BuffonsNeedlePi(){};
        double CalculateApproximation(unsigned int num_trials);
        std::string ID() { return "[Buffon's Needle]";}

};

class GPUMonteCarloPi : public ParallelPi
{
    public:
        GPUMonteCarloPi(){};
        double CalculateApproximation(unsigned int num_trials);
        std::string ID() { return "[GPU Darts]";}
    
};

class HybridMonteCarloPi : public ParallelPi
{
    public:
        HybridMonteCarloPi(float gpu_ratio) : gpu_ratio(gpu_ratio){};
        double CalculateApproximation(unsigned int num_trials);
        std::string ID() { return "[CPU+GPU Darts @ r= "+ std::to_string(gpu_ratio)+" ]";}
    private:
        float gpu_ratio;
};

#endif
