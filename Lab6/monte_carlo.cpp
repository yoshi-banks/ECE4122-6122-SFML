/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-11-08
 * 
 * @brief: Uses OpenMPI to estimate definite integrals using the Monte Carlo method
 */

#include <mpi.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <random>
#include <string>

/**
 * @brief: x^2 function
 * 
 * @param x argument to the function
 * @return double x^2
 */
double function1(double x)
{
    return x * x;
}

/**
 * @brief: gauss function
 * 
 * @param x aregument to the function
 * @return double exp(-x^2)
 */
double function2(double x)
{
    return exp(-x * x);
}

/**
 * @brief: parse user input arguments
 * 
 * @param argc number of args
 * @param argv argument array
 * @param int P which function to use
 * @param long long N how many points used to evaluate
 * @return true if arguments were valid
 * @return false if arguments were invalid
 */
bool parseArgs(int argc, char* argv[], int& P, long long& N)
{
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "-P" && i + 1 < argc)
        {
            P = atoi(argv[++i]);
        }
        else if (arg == "-N" && i + 1 < argc)
        {
            N = atoll(argv[++i]);
        }
    }
    return (P == 1 || P == 2) && N > 0;
}

/**
 * @brief: main function run the OpenMPI Monte-Carlo integral function estimator
 * 
 * @param argc number of arguments
 * @param argv string array of arguments
 * @return int error code
 */
int main(int argc, char* argv[])
{
    int rank, size;
    int P = 0;          // Choose which function
    long long N = 0;    // Num Samples

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Parse arguments
    if (!parseArgs(argc, argv, P, N))
    {
        if (rank == 0)
        {
            std::cerr << "Usage: " << argv[0] << " -P <1|2> -N <num_samples>" << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    // Choose which function to use
    double (*func)(double);
    if (P == 1)
    {
        func = function1;
    }
    else if (P == 2)
    {
        func = function2;
    }
    else
    {
        std::cerr << "P must be either 1 or 2" << std::endl;
        return 1;
    }

    // Integration bounds
    double a = 0.0;
    double b = 1.0;

    // Calculate samples per process
    long long samples_per_process = N / size;
    long long remainder = N % size;

    // Adjust for remainder - add remainder to first couple processes
    long long local_N = samples_per_process;
    if (rank < remainder)
    {
        local_N++;
    }

    // Iniitialize random number generator
    std::random_device rd;
    std::mt19937 gen(rd() + rank);
    std::uniform_real_distribution<double> dist(a, b);

    // MonteCarlo sampling 
    double local_sum = 0.0;
    for (long long i = 0; i < local_N; i++)
    {
        double x = dist(gen);
        local_sum += func(x);
    }

    // Reduce all lcoal sums to get total sum using OpenMPI
    double global_sum = 0.0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Root process computes and prints final result
    if (rank == 0)
    {
        double mean = global_sum / N;
        double integral_estimate = (b - a) * mean;

        std::cout << "The estimate for integral " << P << " is " << integral_estimate << std::endl;
        std::cout << "Bye!" << std::endl;
    }

    MPI_Finalize();
    return 0;
}