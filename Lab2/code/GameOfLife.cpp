/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-09-22
 * 
 * @brief: 
 */

#include "GameOfLife.hpp"

GameOfLife::GameOfLife(int numThreads, int cellSize, int windowWidth, int windowHeight, const std::string& threadingType)
    : window(sf::VideoMode(windowWidth, windowHeight), "Game of Life - ECE Game")
    , cellSize(cellSize)
    , windowWidth(windowWidth)
    , windowHeight(windowHeight)
    , numThreads(numThreads)
    , processingType(threadingType)
    , generationCount(0)
{
    gridWidthSize = windowWidth / cellSize;
    gridHeightSize = windowHeight / cellSize;

    currentGrid.resize(gridHeightSize, std::vector<bool>(gridWidthSize, false));
    nextGrid.resize(gridHeightSize, std::vector<bool>(gridWidthSize, false));

    window.setFramerateLimit(60);
    cellShape.setSize(sf::Vector2f(cellSize - 1, cellSize - 1)); // -1 for grid lines
    cellShape.setFillColor(sf::Color::White);

    initializeGrid();
}

void GameOfLife::initializeGrid()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    for (int y = 0; y < gridHeightSize; ++y)
    {
        for (int x = 0; x < gridWidthSize; ++x)
        {
            currentGrid[y][x] = dis(gen) == 1;
        }
    }
}

int GameOfLife::countNeighbors(int x, int y) const
{
    int count = 0;
    for (int dy = -1; dy <= 1; ++dy)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            if (dx == 0 && dy == 0) continue; // Skip yourself

            int nx = x + dx;
            int ny = y + dy;

            // Handle boundaries (I'm assuming the outside cells are dead)
            if (nx >= 0 && nx < gridWidthSize && ny >= 0 && ny < gridHeightSize)
            {
                if (currentGrid[ny][nx]) count++;
            }
        }
    }
    return count;
}

// Sequential Processing (SEQ)
void GameOfLife::processSequential()
{
    for (int y = 0; y < gridHeightSize; ++y)
    {
        for (int x = 0; x < gridWidthSize; ++x)
        {
            int neighbors = countNeighbors(x, y);
            bool isAlive = currentGrid[y][x];

            if (isAlive)
            {
                // stay alive if neighbors is 2 or 3
                nextGrid[y][x] = (neighbors == 2 || neighbors == 3);
            }
            else
            {
                // make alive if neighbors is 3
                nextGrid[y][x] = (neighbors == 3);
            }
        }
    }
}

// Multithreaded Processing (THRD)
void GameOfLife::processWithThreads()
{
    std::vector<std::thread> threads;
    int rowsPerThread = gridHeightSize / numThreads;

    for (int t = 0; t < numThreads; ++t)
    {
        int startRow = t * rowsPerThread;
        int endRow = (t == numThreads - 1) ? gridHeightSize : (t + 1) * rowsPerThread;

        threads.emplace_back([this, startRow, endRow]()
        {
            for (int y = startRow; y < endRow; ++y)
            {
                for (int x = 0; x < gridWidthSize; ++x)
                {
                    int neighbors = countNeighbors(x, y);
                    bool isAlive = currentGrid[y][x];

                    if (isAlive)
                    {
                        nextGrid[y][x] = (neighbors == 2 || neighbors == 3);
                    }
                    else
                    {
                        nextGrid[y][x] = (neighbors == 3);
                    }
                }
            }
        });
    }

    // Run theads and wait for them to finish
    for (auto& thread : threads)
    {
        thread.join();
    }
}

// OpenMP Processing (OMP)
void GameOfLife::processWithOpenMP()
{
    omp_set_num_threads(numThreads);

    #pragma omp parallel for
    for (int y = 0; y < gridHeightSize; ++y)
    {
        for (int x = 0; x < gridWidthSize; ++x)
        {
            int neighbors = countNeighbors(x, y);
            bool isAlive = currentGrid[y][x];

            if (isAlive)
            {
                nextGrid[y][x] = (neighbors == 2 || neighbors == 3);
            }
            else
            {
                nextGrid[y][x] = (neighbors == 3);
            }
        }
    }
}

void GameOfLife::nextGeneration()
{
    auto start = std::chrono::high_resolution_clock::now();

    if (processingType == "SEQ")
    {
        processSequential();
    }
    else if (processingType == "THRD")
    {
        processWithThreads();
    }
    else if (processingType == "OMP")
    {
        processWithOpenMP();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // Switch grids
    currentGrid.swap(nextGrid);

    // Track performance
    generationTimes.push_back(duration);
    generationCount++;

    if (generationTimes.size() > 100)
    {
        generationTimes.erase(generationTimes.begin());
    }

    // Print performance to terminal every 100 generations
    if (generationCount % 100 == 0 && generationTimes.size() == 100)
    {
        long long totalTime = 0;
        for (long long time : generationTimes)
        {
            totalTime += time;
        }

        std::cout << "100 generations took " << totalTime << " microseconds with ";
        if (processingType == "SEQ")
        {
            std::cout << "single thread." << std::endl;
        }
        else if (processingType == "THRD")
        {
            std::cout << numThreads << " std::threads." << std::endl;
        }
        else if (processingType == "OMP")
        {
            std::cout << numThreads << " OMP threads." << std::endl;
        }
    }
}

void GameOfLife::handleEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed ||
            (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
        {
            window.close();
        }
    }
}

void GameOfLife::render()
{
    window.clear(sf::Color::Black);

    for (int y = 0; y < gridHeightSize; ++y)
    {
        for (int x = 0; x < gridWidthSize; ++x)
        {
            if (currentGrid[y][x])
            {
                cellShape.setPosition(x * cellSize, y * cellSize);
                window.draw(cellShape);
            }
        }
    }

    window.display();
}

void GameOfLife::run()
{
    while (window.isOpen())
    {
        handleEvents();
        nextGeneration();
        // render();
    }
}

