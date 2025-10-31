cmake_minimum_required(VERSION 3.10)
project(Lab4)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")

# Include directories os the compiler can find the header files
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${OPENAL_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/../SFML/include)
link_directories(${PROJECT_SOURCE_DIR}/../SFML/lib)

# Server executable
add_executable(server
    main_server.cpp
    ${PROJECT_SOURCE_DIR}/src/TCPServer.cpp
)
target_link_libraries(server 
    pthread
    sfml-network
    sfml-system
)

# Client executable
add_executable(client
    main_client.cpp
    ${PROJECT_SOURCE_DIR}/src/TCPClient.cpp
)
target_link_libraries(client 
    pthread
    sfml-network
    sfml-system
)

# Custom target to clean log files
add_custom_target(clean-logs
    COMMAND ${CMAKE_COMMAND} -E remove -f server.log
    COMMENT "Removing server.log file"
)

# Custom target to run tests
add_custom_target(run-server
    COMMAND $<TARGET_FILE:server> 61717
    DEPENDS server
    COMMENT "Starting server on port 61717"
)

add_custom_target(run-client
    COMMAND $<TARGET_FILE:client> localhost 61717
    DEPENDS client
    COMMENT "Starting client connecting to localhost:61717"
)