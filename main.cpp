#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdint>

#include "omp.h"
#include "pi.h"
#include "common.h"

int main(int argc, char** argv) {
    // Parse Args
    if (CommonCLI::find_arg_idx(argc, argv, "-h") >= 0) {
        std::cout << "Options:" << std::endl;
        std::cout << "-h: see this help" << std::endl;
        std::cout << "-n <int>: set number of trials" << std::endl;
        std::cout << "-p <int>: set the number of threads. default: max." << std::endl;
        //std::cout << "-o <filename>: set the output file name" << std::endl;
        std::cout << "-t <int>: 1 - integral (default), " << std::endl;
        std::cout << "          2 - montecarlo darts (cpu) " << std::endl;
        std::cout << "          3 - montecarlo darts (gpu), " << std::endl;
        std::cout << "          4 - montecarlo darts (gpu+cpu)." << std::endl;
        std::cout << "-r <float>: ratio for (gpu+cpu)." << std::endl;
        return 0;
    }

    // Open Output File
    //    char* savename = find_string_option(argc, argv, "-o", nullptr);
    //    std::ofstream fsave(savename);
    std::int64_t num_trials = (std::int64_t)CommonCLI::find_ull_arg(argc, argv, "-n", 1000);
    int type = CommonCLI::find_int_arg(argc,argv, "-t",1);
    int num_cpu = CommonCLI::find_int_arg(argc,argv,"-p",-1);
    double gpu_ratio = CommonCLI::find_double_arg(argc,argv,"-r",0.90);


    if(num_trials > UINT64_MAX ) // 4'294'967'295
    {
        std::cout << "Number of trials limited to: " << UINT32_MAX << std::endl;
        exit(0);
    }

    if(num_cpu > 0)
    {
        omp_set_num_threads(num_cpu);
    }

    std::cout << "Trials= " << num_trials << " with num threads= " << num_cpu << std::endl; 

    ParallelPi * pi_solver;
    switch(type)
    {
        case 1:
            pi_solver = new IntegralPi();
            break;
        case 2:
            pi_solver = new MonteCarloPi();
            break;
        case 3:
            pi_solver = new GPUMonteCarloPi();
            break;
        case 4:
            pi_solver = new HybridMonteCarloPi(gpu_ratio);
            break;
        case 5:
            pi_solver = new BuffonsNeedlePi();
            break;
        default:
            std::cout << "Error: Invalid Solver Type. Try -h for help!" << std::endl;
            exit(0);
            break;
    }

    std::cout << "Using Method: " << pi_solver->ID() << std::endl;

    auto start_time = std::chrono::steady_clock::now();
    double pi_approx = pi_solver->CalculateApproximation(num_trials);
    auto end_time = std::chrono::steady_clock::now();

    std::chrono::duration<double> diff = end_time - start_time;
    std::cout << "Pi approximation: " << pi_approx << " with error " << ParallelPi::CalculateError(pi_approx) << " in " << diff.count() << " seconds." <<std::endl;
    
    delete pi_solver;

}