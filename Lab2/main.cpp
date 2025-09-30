/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-09-22
 * 
 * @brief: 
 */

#include <iostream>
#include <string>
#include <cstdlib>

#include "code/GameOfLife.hpp"

int NUM_THREADS_DEFAULT = 8;
int CELL_SIZE_DEFAULT = 5;
int WINDOW_WIDTH_DEFAULT = 800;
int WINDOW_HEIGHT_DEFAULT = 600;
std::string PROCESSING_TYPE_DEFAULT = "THRD";

/**
 * @brief: Configuration structure to hold command line arguments
 * 
 */
struct Config
{
    int numThreads = NUM_THREADS_DEFAULT;
    int cellSize = CELL_SIZE_DEFAULT;
    int windowWidth = WINDOW_WIDTH_DEFAULT;
    int windowHeight = WINDOW_HEIGHT_DEFAULT;
    std::string processingType = PROCESSING_TYPE_DEFAULT;
};

/**
 * @brief: Parse command line arguments
 * 
 * @param argc number of arguments
 * @param argv character array of arguments
 * @return Config 
 */
Config parseArguments(int argc, char* argv[])
{
    Config config;

    int numThreads = -1;
    int cellSize = -1;
    int windowWidth = -1;
    int windowHeight = -1;
    std::string processingType;

    bool n_set = false;
    bool c_set = false;
    bool x_set = false;
    bool y_set = false;
    bool t_set = false;

    // Parse command line arguments
    for (int i = 1; i < argc; i += 2)
    {
        if (i + 1 >= argc)
        {
            std::cerr << "Error: Missing value for argument " << argv[i] << std::endl;
            exit(1);
        }
        std::string flag = argv[i];
        std::string value = argv[i + 1];

        if (flag == "-n")
        {
            numThreads = std::stoi(value);
            if (numThreads <= 0)
            {
                std::cerr << "Error: Number of threads must be a positive integer. NumThreads was: " << numThreads << std::endl;
                exit(1);
            }
            n_set = true;
        }
        else if (flag == "-c")
        {
            cellSize = std::stoi(value);
            if (cellSize <= 0)
            {
                std::cerr << "Error: Cell size must be a positive integer. CellSize was: " << cellSize << std::endl;
                exit(1);
            }
            c_set = true;
        }
        else if (flag == "-x")
        {
            windowWidth = std::stoi(value);
            if (windowWidth <= 0)
            {
                std::cerr << "Error: Window width must be a positive integer. Window width was: " << windowWidth << std::endl;
                exit(1);
            }
            x_set = true;
        }
        else if (flag == "-y")
        {
            windowHeight = std::stoi(value);
            if (windowHeight <= 0)
            {
                std::cerr << "Error: Window height must be a positive integer. Window height was: " << windowHeight << std::endl;
                exit(1);
            }
            y_set = true;
        }
        else if (flag == "-t")
        {
            processingType = value;
            if (processingType != "SEQ" && processingType != "THRD" && processingType != "OMP")
            {
                std::cerr << "Error: Invalid threading type. Must be SEQ, THRD, or OMP. Threading type was: " << processingType << std::endl;
                exit(1);
            }
            t_set = true;
        }
        else
        {
            std::cerr << "Error: Unknown flag " << flag << std::endl;
            exit(1);
        }
    }

    // Set default values if not set
    if (!n_set) numThreads = NUM_THREADS_DEFAULT;
    if (!c_set) cellSize = CELL_SIZE_DEFAULT;
    if (!x_set) windowWidth = WINDOW_WIDTH_DEFAULT;
    if (!y_set) windowHeight = WINDOW_HEIGHT_DEFAULT;
    if (!t_set) processingType = PROCESSING_TYPE_DEFAULT;

    std::cout << "n: " << numThreads << "c: " << cellSize << ", x: " << windowWidth << ", y: " << windowHeight << ", t: " << processingType << std::endl;
    
    config.numThreads = numThreads;
    config.cellSize = cellSize;
    config.windowWidth = windowWidth;
    config.windowHeight = windowHeight;
    config.processingType = processingType;

    return config;
}

/**
 * @brief: Main function
 * 
 * @param argc number of arguments
 * @param argv character array of arguments
 * @return int 
 */
int main(int argc, char* argv[])
{
    Config config = parseArguments(argc, argv);

    std::cout << "Starting Game of Life with:" << std::endl;
    std::cout << "    Processing: " << config.processingType << std::endl;
    if (config.processingType != "SEQ")
    {
        std::cout << "    Threads: " << config.numThreads << std::endl;
    }
    std::cout << "    Cell Size: " << config.cellSize << std::endl;
    std::cout << "    Window Size: " << config.windowWidth << "x" << config.windowHeight << std::endl;

    GameOfLife game(config.numThreads, 
                    config.cellSize, 
                    config.windowWidth, 
                    config.windowHeight, 
                    config.processingType);

    game.run();

    return 0;
}
