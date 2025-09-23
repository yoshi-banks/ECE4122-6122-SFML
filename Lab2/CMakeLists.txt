cmake_minimum_required(VERSION 3.10)

project(Lab2 VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the include directories for OpenAL
include_directories(${OPENAL_INCLUDE_DIR})

# Add source files
set(SOURCES
    ${PROJECT_SOURCE_DIR}/code/GameOfLife.cpp
)

# Find packages
find_package(OpenMP REQUIRED)
find_package(Threads REQUIRED)

# Create executable
add_executable(${PROJECT_NAME} 
    main.cpp 
    ${SOURCES}
)

include_directories(${PROJECT_SOURCE_DIR}/../SFML/include)

link_directories(${PROJECT_SOURCE_DIR}/../SFML/lib)

# Link SFML libraries
target_link_libraries(${PROJECT_NAME}
    OpenMP::OpenMP_CXX
    Threads::Threads
    sfml-graphics 
    sfml-window 
    sfml-system 
    pthread
)