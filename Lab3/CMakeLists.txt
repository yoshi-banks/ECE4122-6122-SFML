cmake_minimum_required(VERSION 3.10)

project(Lab3 VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the include directories for OpenAL
include_directories(${OPENAL_INCLUDE_DIR})


# Find packages
find_package(OpenGL REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Threads REQUIRED)

# Include directories from external
include_directories(
    ${PROJECT_SOURCE_DIR}/../external/AntTweakBar-1.16/include
    ${PROJECT_SOURCE_DIR}/../external/glfw-3.1.2/include
    ${PROJECT_SOURCE_DIR}/../external/glm-0.9.7.1
    ${PROJECT_SOURCE_DIR}/../external/glew-1.13.0/include
    ${PROJECT_SOURCE_DIR}/../external/assimp-3.0.1270/include
    ${PROJECT_SOURCE_DIR}/../bullet-2.81-rev2613/src
)

# Add source files
set(OGL_LIBS
    # ${OPENGL_LIBRARY}
    OpenGL::GL
    glfw
    GLEW_1130
    assimp
)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/code/Application.cpp
    ${PROJECT_SOURCE_DIR}/code/Camera.cpp
    ${PROJECT_SOURCE_DIR}/code/Shader.cpp
    ${PROJECT_SOURCE_DIR}/code/Model.cpp
)

add_executable(tutorial01_first_window
    tutorial01.cpp
)

target_link_libraries(tutorial01_first_window
    PRIVATE
    ${OGL_LIBS}
)

# Create executable
add_executable(${PROJECT_NAME} 
    main.cpp 
    ${SOURCES}
)

include_directories(${PROJECT_SOURCE_DIR}/../SFML/include)

link_directories(${PROJECT_SOURCE_DIR}/../SFML/lib)

# Link SFML libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    ${OGL_LIBS}
    # OpenMP::OpenMP_CXX
    # Threads::Threads
    # sfml-graphics 
    # sfml-window 
    # sfml-system 
    # pthread
)