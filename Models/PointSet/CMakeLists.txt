add_library(Point Point.cpp)
add_library(PointSet PointSet.cpp)

target_link_libraries(PointSet PUBLIC Point)