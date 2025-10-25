

#include <cuda_runtime.h>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "heat_solver.cuh"

// CUDA error checking macro
#define CUDA_CHECK(call) \
    do { \
        cudaError_t err = call; \
        if (err != cudaSuccess) { \
            std::cerr << "CUDA error in " << __FILE__ << " at line " << __LINE__ << ": " \
                      << cudaGetErrorString(err) << std::endl; \
            exit(EXIT_FAILURE); \
    } \
} while (0)

// CUDA kernel for Jacobi iteration
__global__ void jacobiKernel(const double* h, double* g, int N)
{
    // Calculate global thread indices
    int i = blockIdx.x * blockDim.x + threadIdx.x + 1; // +1 to skip boundary
    int j = blockIdx.y * blockDim.y + threadIdx.y + 1; // +1 to skip boundary

    int gridSize = N + 2; // Total grid size including boundaries

    // Only process interior points
    if (i < N + 1 && j < N + 1)
    {
        int idx = i * gridSize + j;

        // Compute average of four neighbors
        g[idx] = 0.25 * (h[(i-1) * gridSize + j] +     // Top
                         h[(i+1) * gridSize + j] +     // Bottom
                         h[i * gridSize + (j-1)] +     // Left
                         h[i * gridSize + (j+1)]);     // Right
    }
}

// Initialize the temperature grid
void initializeGrid(double* h, int N)
{
    int gridSize = N + 2; 

    // Initialize all points to 0
    for (int i = 0; i < gridSize; i++)
    {
        for (int j = 0; j < gridSize; j++)
        {
            h[i * gridSize + j] = 0.0;
        }
    }

    // Set boundary conditions
    // All edges at 20 deg C
    for (int i = 0; i < gridSize; i++)
    {
        h[0 * gridSize + i] = 20.0;              // Top edge
        h[(N+1) * gridSize + i] = 20.0;          // Bottom edge
        h[i * gridSize + 0] = 20.0;              // Left edge
        h[i * gridSize + (N+1)] = 20.0;          // Right edge
    }

    // Hot segment at 100 deg C on top edge (4 ft segment on 10 ft side)
    // the segment is then 4/10 = 0.4 of the edge, centered
    double segmentRatio = 0.4;
    int segmentLength = (int)(gridSize * segmentRatio);
    int start = (gridSize - segmentLength) / 2; // center the segment
    int end = start + segmentLength;
    for (int j = start; j < end; j++)
    {
        h[0 * gridSize + j] = 100.0; // Top edge hot segment
    }
}

// Write results to CSV file
void writeResultsToCSV(const double* h, int N, const char* filename)
{
    std::ofstream outFile(filename);

    if (!outFile.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    int gridSize = N + 2;

    // Write all points including boundaries
    outFile << std::fixed << std::setprecision(6);
    for (int i = 0; i < gridSize; i++)
    {
        for (int j = 0; j < gridSize; j++)
        {
            outFile << h[i * gridSize + j];
            if (j < gridSize - 1)
            {
                outFile << ",";
            }
        }
        outFile << "\n";
    }

    outFile.close();
    std::cout << "Results written to " << filename << std::endl;
}

// Main CUDA solver funtion
float solveHeatCUDA(int N, int numIter, const char* outputFilename)
{
    int gridSize = N + 2; // Total grid size including boundaries
    size_t arraySize = gridSize * gridSize * sizeof(double);

    // Allocate host memory
    double* h_host = new double[gridSize * gridSize];
    double* g_host = new double[gridSize * gridSize];

    // Initialize grid with boundary conditions
    initializeGrid(h_host, N);

    // Allocate device memory
    double *d_h, *d_g;
    CUDA_CHECK(cudaMalloc(&d_h, arraySize));
    CUDA_CHECK(cudaMalloc(&d_g, arraySize));

    // Copy initial data to device
    CUDA_CHECK(cudaMemcpy(d_h, h_host, arraySize, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_g, h_host, arraySize, cudaMemcpyHostToDevice));

    // Define block and grid dimensions
    dim3 blockDim(16, 16);
    dim3 gridDim((N + blockDim.x - 1) / blockDim.x,
                 (N + blockDim.y - 1) / blockDim.y);

    // Create CUDA events for timing
    cudaEvent_t start, stop;
    CUDA_CHECK(cudaEventCreate(&start));
    CUDA_CHECK(cudaEventCreate(&stop));

    // Start timing
    CUDA_CHECK(cudaEventRecord(start));

    // Perform iterations
    for (int iter = 0; iter < numIter; iter++)
    {
        // Compute new values in d_g from d_h
        jacobiKernel<<<gridDim, blockDim>>>(d_h, d_g, N);
        CUDA_CHECK(cudaGetLastError());

        // Swap pointers (d_h now points to new values)
        double* temp = d_h;
        d_h = d_g;
        d_g = temp;
    }

    // Wait for all kernels to complete
    CUDA_CHECK(cudaDeviceSynchronize());

    // Stop timing
    CUDA_CHECK(cudaEventRecord(stop));
    CUDA_CHECK(cudaEventSynchronize(stop));

    // Calculate elapsed time
    float milliseconds = 0;
    CUDA_CHECK(cudaEventElapsedTime(&milliseconds, start, stop));

    // Copy results back to host
    CUDA_CHECK(cudaMemcpy(h_host, d_h, arraySize, cudaMemcpyDeviceToHost));

    // Write results to CSV
    writeResultsToCSV(h_host, N, outputFilename);

    // Cleanup
    CUDA_CHECK(cudaEventDestroy(start));
    CUDA_CHECK(cudaEventDestroy(stop));
    CUDA_CHECK(cudaFree(d_h));
    CUDA_CHECK(cudaFree(d_g));
    delete[] h_host;
    delete[] g_host;

    return milliseconds;
}