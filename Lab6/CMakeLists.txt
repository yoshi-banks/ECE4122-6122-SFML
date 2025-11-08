cmake_minimum_required(VERSION 3.10)

project(Lab6 VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find MPI
find_package(MPI REQUIRED)

# Add executable
add_executable(monte_carlo monte_carlo.cpp)

# Link MPI libraries
target_link_libraries(monte_carlo MPI::MPI_CXX)

# compile options for optimization
target_compile_options(monte_carlo PRIVATE -O3 -Wall)

# Print MPI information 
message(STATUS "MPI_CXX_COMPILER: ${MPI_CXX_COMPILER}")
message(STATUS "MPI_CXX_INCLUDE_DIRS: ${MPI_CXX_INCLUDE_DIRS}")
message(STATUS "MPI_CXX_LIBRARIES: ${MPI_CXX_LIBRARIES}")
