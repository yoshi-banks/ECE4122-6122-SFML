/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-10-25
 * 
 * @brief: cuda header for 2D steady state heat conduction solver
 */

#pragma once

// FUnction to solve the 2D heat conduction problem using CUDA
float solveHeatCUDA(int numPoints, int numIter, const char* outputFilename);