/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-10-25
 * 
 * @brief: main function for 2D steady state heat conduction solver using CUDA
 */

#include <iostream>

#include "src/heat_solver.cuh"

int NUM_POINTS_DEFAULT = 256;
int NUM_ITER_DEFAULT = 10000;

/**
 * @brief: Configuration structure to hold command line arguments
 * 
 */
struct Config
{
    int numPoints; // number of N x N interior points
    int numIter;   // number of iterations
    bool quitFlag = false;
};

/**
 * @brief: Parse command line arguments
 * 
 * @param argc 
 * @param argv 
 * @return Config 
 */
Config parseArguments(int argc, char* argv[])
{
    Config config;

    int numPoints = -1;
    int numIter = -1;

    // Parse command line arguments
    for (int i = 1; i < argc; i += 2)
    {
        // Exit early if quit flag is found
        std::string flag = argv[i];
        if (flag == "-q" || flag == "--quit")
        {
            // Quit flag does not require a value
            config.quitFlag = true;
            break;
        }

        // Ensure there is a value for the flag
        if (i + 1 >= argc)
        {
            std::cerr << "Error: Missing value for argument " << argv[i] << std::endl;
            exit(1);
        }
        std::string value = argv[i + 1];

        // Parse known flags
        if (flag == "-N")
        {
            numPoints = std::stoi(value);
            if (numPoints <= 0)
            {
                std::cerr << "Error: Number of interior points must be a positive integer. NumPoints was: " << numPoints << std::endl;
                exit(1);
            }
        }
        else if (flag == "-I" || flag == "--iterations")
        {
            numIter = std::stoi(value);
            if (numIter <= 0)
            {
                std::cerr << "Error: Number of iterations must be a positive integer. NumIter was: " << numIter << std::endl;
                exit(1);
            }
        }
        else
        {
            std::cerr << "Error: Unknown flag " << flag << std::endl;
            exit(1);
        }
    }
    
    // Set default values if not set
    if (numPoints == -1) numPoints = NUM_POINTS_DEFAULT; // default value
    if (numIter == -1) numIter = NUM_ITER_DEFAULT;    // default value

    std::cout << "N: " << numPoints << ", I: " << numIter << std::endl;

    config.numPoints = numPoints;
    config.numIter = numIter;

    return config;
}

/**
 * @brief: Main function. Parses arguments, has some console output, and runs the CUDA solver
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char* argv[])
{
    Config config = parseArguments(argc, argv);

    if (config.quitFlag)
    {
        std::cout << "Program terminated by user." << std::endl;
        return 0;
    }

    std::cout << "Starting simulation with:" << std::endl;
    std::cout << "    Interior Points: " << config.numPoints << std::endl;
    std::cout << "    Iterations: " << config.numIter << std::endl;
    std::cout << std::endl;

    // Solve the heat conduction problem using CUDA
    float executionTime = solveHeatCUDA(config.numPoints, config.numIter, "finalTemperatures.csv");

    // Output the execution time
    std::cout << std::endl;
    std::cout << "CUDA Execution Time: " << executionTime << " milliseconds" << std::endl;

    return 0;
}