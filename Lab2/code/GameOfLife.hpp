/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-09-22
 * 
 * @brief: 
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

class GameOfLife
{
    private:
        // Configuration
        int cellSize;
        int windowWidth;
        int windowHeight;
        std::string processingType;

        int gridWidthSize;
        int gridHeightSize;
        int numThreads;

        std::vector<std::vector<bool>> currentGrid;
        std::vector<std::vector<bool>> nextGrid;

        // Performance
        std::vector<long long> generationTimes;
        int generationCount;

        // Graphics
        sf::RenderWindow window;
        sf::RectangleShape cellShape;

    public:
        GameOfLife(int numThreads, int cellSize, int windowWidth, int windowHeight, const std::string& threadingType);
        void run();

    private:
        void handleEvents();
        void render();
        void initialize();
        void initializeGrid();
        int countNeighbors(int x, int y) const;
        void processSequential();
        void processWithThreads();
        void processWithOpenMP();
        void nextGeneration();
};