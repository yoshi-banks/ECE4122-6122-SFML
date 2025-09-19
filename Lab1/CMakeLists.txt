cmake_minimum_required(VERSION 3.16)
project(Lab1)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_BUILD_AUDIO OFF)          # Disable audio module
set(SFML_BUILD_EXAMPLES OFF)
set(SFML_BUILD_DOC OFF)
set(SFML_BUILD_TESTS OFF)

# add SFML as subdirectory 
add_subdirectory(SFML)

# Define a common output directory
set(COMMON_OUTPUT_DIR "${CMAKE_BINARY_DIR}/output")

# Set output directories to be in the same directory as CMakeLists.txt
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${COMMON_OUTPUT_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${COMMON_OUTPUT_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${COMMON_OUTPUT_DIR}/lib)

# Add source files
set(SOURCES
    ${PROJECT_SOURCE_DIR}/code/ECE_Buzzy.cpp
    ${PROJECT_SOURCE_DIR}/code/ECE_Enemy.cpp
    ${PROJECT_SOURCE_DIR}/code/ECE_LaserBlast.cpp
    ${PROJECT_SOURCE_DIR}/code/BuzzyDefenderGame.cpp
)

# Create executable
add_executable(${PROJECT_NAME} 
    main.cpp 
    ${SOURCES}
)

# Link SFML libraries
target_link_libraries(${PROJECT_NAME}
    sfml-graphics 
    sfml-window 
    sfml-system 
)