#include "pi.h"

double MonteCarloPi::CalculateApproximation(unsigned int num_trials)   
{ 
    unsigned int count = 0; //declared outside parallel region.
    #pragma omp parallel
    {
        std::random_device rseed;
        std::mt19937 rgen(rseed()); // mersenne_twister
        std::uniform_real_distribution<double> rdist(0.0,1.0);

        #pragma omp for reduction(+:count)
        for(std::int64_t i=0; i < num_trials; ++i)
        {
            double random_x = rdist(rgen);
            double random_y = rdist(rgen);
            if(random_x*random_x + random_y*random_y <= 1.0)
            {
                count = count + 1; //if we are in the circle increment the count.
            }
        }
    }
    return ((double)count/(double)num_trials) * 4.0;    
}

double IntegralPi::CalculateApproximation(unsigned int num_trials)   
{ 
    double sum = 0;
    double step = 1.0/(double)num_trials;
    double x=0;

    // Approx. using the Integral from 0 to 1 of 4.0/(1+x^2) dx = Pi
    #pragma omp parallel for reduction(+:sum) private(x)
    for(std::int64_t i=0; i < num_trials; ++i)
    {
        x = (i + 0.5) * step;
        sum += 4.0/(1.0 + x * x); 
    }

    return sum * step;
}

double BuffonsNeedlePi::CalculateApproximation(unsigned int num_trials)
{
    double needle_length = 0.9;
    unsigned int hit_count = 0;

    #pragma omp parallel
    {
        std::random_device rseed;
        std::mt19937 rgen(rseed()); // mersenne_twister
        std::uniform_real_distribution<double> rdist(0.0,1.0);

        #pragma omp for reduction(+:hit_count)
        for (std::int64_t i=0; i<num_trials; i++)
        {
            double random_angle = rdist(rgen)*(2*ACTUAL_PI); 
            double x = rdist(rgen); 
            double y = (needle_length/2.0) * sin (random_angle);

            if (x<=y)
            {
                hit_count += 1;
            }
        }
    }
    //these trials should converge to Pr = 2/pi
    return (needle_length*(double)num_trials)/(2.0*(double)hit_count);
}
