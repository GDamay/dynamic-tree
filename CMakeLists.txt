cmake_minimum_required(VERSION 3.10)

# set the project name
project(Dynamic_Tree VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(Models/PointSet)
add_subdirectory(Models/Tree)

# add the executable
add_executable(dynamic_tree main.cpp)

target_link_libraries(dynamic_tree PUBLIC Point)
target_link_libraries(dynamic_tree PUBLIC PointSet)
target_link_libraries(dynamic_tree PUBLIC Vertex)
target_link_libraries(dynamic_tree PUBLIC Tree)

target_include_directories(dynamic_tree PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/PointSet")

target_include_directories(dynamic_tree PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/Tree"
                          )