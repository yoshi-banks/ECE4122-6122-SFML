/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-10-25
 * 
 * @brief: main function for 2D steady state heat conduction solver using CUDA
 */

#include <iostream>
#include <sstream>
#include <string>

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
    bool valid = true;
};

/**
 * @brief: Parse command line arguments
 * 
 * @param input Input string containing arguments
 * @return Config Configuration with parsed values
 */
Config parseInputLine(const std::string& input)
{
    Config config;
    std::istringstream iss(input);
    std::string token;

    while (iss >> token)
    {
        if (token == "-q" || token == "--quit")
        {
            config.quitFlag = true;
            break;
        }
        else if (token == "-N")
        {
            int N;
            if (!(iss >> N) || N <= 0)
            {
                std::cerr << "Error: Invalid value for -N" << std::endl;
                config.valid = false;
                return config;
            }
            config.numPoints = N;
        }
        else if (token == "-I" || token == "--iterations")
        {
            int I;
            if (!(iss >> I) || I <= 0)
            {
                std::cerr << "Error: Invalid value for " << token << std::endl;
                config.valid = false;
                return config;
            }
            config.numIter = I;
        }
        else
        {
            std::cerr << "Error: Unknown flag " << token << std::endl;
            config.valid = false;
            return config;
        }
    }
    return config;
}

/**
 * @brief: Main function. Parses arguments, has some console output, and runs the CUDA solver
 * 
 * @return int 
 */
int main()
{
    std::cout << "=== 2D Steady State Heat Conduction Solver ===" << std::endl;
    std::cout << "Default parameters: -N " << NUM_POINTS_DEFAULT 
              << " -I " << NUM_ITER_DEFAULT << std::endl;
    std::cout << std::endl;

    while (true)
    {
        std::cout << "Enter arguments (e.g. -N 256 -I 10000) or -q to quit: ";
        std::string inputLine;
        std::getline(std::cin, inputLine);

        // Skip empty input
        if (inputLine.empty())
        {
            continue;
        }

        Config config = parseInputLine(inputLine);

        if (config.quitFlag)
        {
            std::cout << "Program terminated by user." << std::endl;
            break;
        }

        // Skip if parsing failed
        if (!config.valid)
        {
            std::cout << "Please try again." << std::endl;
            std::cout << std::endl;
            continue;
        }

        std::cout << "Starting simulation with:" << std::endl;
        std::cout << "    Interior Points: " << config.numPoints << " x " << config.numPoints << std::endl;
        std::cout << "    Iterations: " << config.numIter << std::endl;
        std::cout << std::endl;
        
        try 
        {
            // Solve the heat conduction problem using CUDA
            float executionTime = solveHeatCUDA(config.numPoints, config.numIter, "finalTemperatures.csv");

            // Output the execution time
            std::cout << std::endl;
            std::cout << "CUDA Execution Time: " << executionTime << " milliseconds" << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "An error occurred during execution: " << e.what() << std::endl;
        }
        std::cout << std::endl;
    }

    return 0;
}