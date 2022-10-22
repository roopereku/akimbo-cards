cmake_minimum_required(VERSION 3.0)
project(cards)

set(CMAKE_BUILD_TYPE Debug)
add_definitions(-DDEBUG)

FILE(GLOB CardsSources *.cc)

include_directories(akimbo)
add_subdirectory(akimbo)
add_executable(cards ${CardsSources})
add_dependencies(cards akimbo)
target_link_libraries(cards akimbo)

# Copy the resources to where the binary is
FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})
