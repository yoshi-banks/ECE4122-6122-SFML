/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-09-29
 * 
 * @brief: Game of Life implementation file
 */

#include "GameOfLife.hpp"

/**
 * @brief: Construct a new Game Of Life:: Game Of Life object
 * 
 * @param numThreads number of threads to use for THRD and OMP processing
 * @param cellSize size of cell sprite objects
 * @param windowWidth width of the window in pixels
 * @param windowHeight height of the window in pixels
 * @param processingType type of processing to use
 */
GameOfLife::GameOfLife(int numThreads, int cellSize, int windowWidth, int windowHeight, const std::string& processingType)
    : window(sf::VideoMode(windowWidth, windowHeight), "Game of Life - ECE Game")
    , cellSize(cellSize)
    , windowWidth(windowWidth)
    , windowHeight(windowHeight)
    , numThreads(numThreads)
    , processingType(processingType)
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

/**
 * @brief: Initialize the grid with random live/dead cells
 * 
 */
void GameOfLife::initializeGrid()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    // Randomly initialize the grid
    for (int y = 0; y < gridHeightSize; ++y)
    {
        for (int x = 0; x < gridWidthSize; ++x)
        {
            currentGrid[y][x] = dis(gen) == 1;
        }
    }
}

/**
 * @brief: Count the number of live neighbors for a given cell
 * 
 * @param x width index of the cell
 * @param y height index of the cell
 * @return int number of neighbors
 */
int GameOfLife::countNeighbors(const int x, const int y) const
{
    int count = 0;
    for (int dy = -1; dy <= 1; ++dy)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            if (dx == 0 && dy == 0) 
            {
                continue; // Skip yourself
            }

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

/**
 * @brief: Process the next generation using sequential processing
 * 
 */
void GameOfLife::processSequential()
{
    // for each cell, count neighbors and apply rules
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

/**
 * @brief: Process the next generation using std::thread processing
 * 
 */
void GameOfLife::processWithThreads()
{
    // create a vector of threds to process rows in parallel
    std::vector<std::thread> threads;
    int rowsPerThread = gridHeightSize / numThreads;

    for (int t = 0; t < numThreads; ++t)
    {
        // have start row be index * rowsPerThread
        int startRow = t * rowsPerThread;
        // have end row be (index + 1) * rowsPerThread unless it's the last thread of the vector then have it go to the end of the grid
        int endRow = (t == numThreads - 1) ? gridHeightSize : (t + 1) * rowsPerThread;

        // emplace thread lambda function function to process a chunk of rows
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

    // Run theads and wait for them all to finish
    for (auto& thread : threads)
    {
        thread.join();
    }
}

/**
 * @brief: Process the next generation using OpenMP processing
 * 
 */
void GameOfLife::processWithOpenMP()
{
    // set the number of threads for OpenMP
    omp_set_num_threads(numThreads);

    // parallelize the outer loop with OpenMP
    // similiar loop to std::thread version but with omp pragmas
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

/**
 * @brief: Advance to the next generation and track performance
 * 
 */
void GameOfLife::nextGeneration()
{
    // Start timing

    std::chrono::_V2::system_clock::time_point start;
    std::chrono::_V2::system_clock::time_point end;
    if (processingType == "SEQ")
    {
        start = std::chrono::high_resolution_clock::now();
        processSequential();
        end = std::chrono::high_resolution_clock::now();
    }
    else if (processingType == "THRD")
    {
        start = std::chrono::high_resolution_clock::now();
        processWithThreads();
        end = std::chrono::high_resolution_clock::now();
    }
    else if (processingType == "OMP")
    {
        start = std::chrono::high_resolution_clock::now();
        processWithOpenMP();
        end = std::chrono::high_resolution_clock::now();
    }
    else 
    {
        std::cerr << "Error: Unknown processing type " << processingType << std::endl;
        exit(1);
    }

    // calculate timing
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // Switch grids
    currentGrid.swap(nextGrid);

    // Track performance
    generationTimes.push_back(duration);
    generationCount++;

    // Keep only the last 100 generation times
    if (generationTimes.size() > 100)
    {
        // TODO optimize this with a circular buffer if needed
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

/**
 * @brief: Handle window events (close, escape key)
 * 
 */
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

/**
 * @brief: Render the current grid to the window
 * 
 */
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

/**
 * @brief: Main loop to run the Game of Life simulation
 * 
 */
void GameOfLife::run()
{
    while (window.isOpen())
    {
        handleEvents();
        nextGeneration();
        render();
    }
}

