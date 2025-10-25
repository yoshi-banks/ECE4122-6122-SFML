cmake_minimum_required(VERSION 3.10)

project(Lab4 VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable CUDA language
enable_language(CUDA)

# Set CUDA standard
set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# Enable CUDA separable compilation
set(CMAKE_CUDA_SEPARABLE_COMPILATION ON)

# Add source files
set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/heat_solver.cu
)

# Create executable
add_executable(${PROJECT_NAME} 
    main.cpp 
    ${SOURCES}
)

# Set CUDA architectures (adjust based on the GPU)
set_property(TARGET ${PROJECT_NAME} PROPERTY CUDA_ARCHITECTURES 75 86 89)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}
)

# Set CUDA flags
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -arch=sm_75")