cmake_minimum_required(VERSION 3.16)
project(MyProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define a common output directory
set(COMMON_OUTPUT_DIR "${CMAKE_BINARY_DIR}/output")

# Set output directories to be in the same directory as CMakeLists.txt
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${COMMON_OUTPUT_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${COMMON_OUTPUT_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${COMMON_OUTPUT_DIR}/lib)

# Find SFML
find_package(PkgConfig REQUIRED)
pkg_check_modules(SFML2 REQUIRED sfml-all>=2.1)

# Create executable
add_executable(${PROJECT_NAME} main.cpp ${SOURCES})

# Link SFML libraries
target_link_libraries(${PROJECT_NAME} ${SFML2_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${SFML2_INCLUDE_DIRS})
target_compile_options(${PROJECT_NAME} PRIVATE ${SFML2_CFLAGS_OTHER})