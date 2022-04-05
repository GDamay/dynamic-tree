add_library(Vertex Vertex.cpp)
add_library(Tree Tree.cpp)

target_link_libraries(Vertex PUBLIC Point)
target_link_libraries(Vertex PUBLIC PointSet)
target_link_libraries(Tree PUBLIC Point)
target_link_libraries(Tree PUBLIC PointSet)
target_link_libraries(Tree PUBLIC Vertex)

target_include_directories(Vertex PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/PointSet"
                          )