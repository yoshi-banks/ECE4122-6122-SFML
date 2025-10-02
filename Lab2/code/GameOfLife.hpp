/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-09-29
 * 
 * @brief: Game of Life implementation file
 * The Game of Life (an example of a cellular automaton) is played on an infinite two-dimensional 
 * rectangular grid of cells. Each cell can be either alive or dead. The status of each cell changes 
 * each turn of the game (also called a generation) depending on the statuses of that cell's 8 
 * neighbors. Neighbors of a cell are cells that touch that cell, either horizontal, vertical, or diagonal 
 * from that cell.
 * 
 * The initial pattern is the first generation. The second generation evolves from applying the rules 
 * simultaneously to every cell on the game board, i.e. births and deaths happen simultaneously. 
 * Afterwards, the rules are iteratively applied to create future generations. For each generation 
 * of the game, a cell's status in the next generation is determined by a set of rules. These simple 
 * rules are as follows:
 * 
 * • If the cell is alive, then it stays alive if it has either 2 or 3 live neighbors 
 * • If the cell is dead, then it springs to life only in the case that it has 3 live neighbors
 * There are, of course, as many variations to these rules as there are different combinations of 
 * numbers to use for determining when cells live or die. Conway tried many of these different 
 * variants before settling on these specific rules. Some of these variations cause the populations 
 * to quickly die out, and others expand without limit to fill up the entire universe, or some large 
 * portion thereof. 
 */

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <string>
#include <cstring>
#include <omp.h>
#include <SFML/Graphics.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class GameOfLife
{
    private:
        // Configuration
        int cellSize;
        int windowWidth;
        int windowHeight;
        std::string processingType;
        int numThreads;

        // grid settings
        int gridWidthSize;
        int gridHeightSize;
        std::vector<std::vector<bool>> currentGrid;
        std::vector<std::vector<bool>> nextGrid;

        // Performance
        std::vector<long long> generationTimes;
        int generationCount;

        // Graphics
        sf::RenderWindow window;
        sf::RectangleShape cellShape;

        // Thread pool for std::thread
        std::vector<std::thread> threadPool;
        std::queue<std::function<void()>> taskQueue;
        std::mutex queueMutex;
        std::condition_variable condition;
        std::atomic<bool> stopPool;
        std::atomic<int> activeTasks;

    public:
        GameOfLife(int numThreads, int cellSize, int windowWidth, int windowHeight, const std::string& threadingType);
        ~GameOfLife();
        void run();

    private:
        void handleEvents();
        void render();
        void initialize();
        void initializeGrid();
        int countNeighbors(const int x, const int y) const;
        void processSequential();
        void processWithThreads();
        void processWithOpenMP();
        void nextGeneration();

        // Thread pool management
        void initializeThreadPool();
        void shutdownThreadPool();
        void workerThread();
        void enqueueTask(const std::function<void()> task);
        void waitForCompletion();
};