cmake_minimum_required(VERSION 2.8.8)

project(TriangleMesh)

if(NOT DEFINED TRIANGLE_MESH)
    set(TRIANGLE_MESH ${CMAKE_CURRENT_SOURCE_DIR})
endif()
if(NOT DEFINED TRIANGLE)
    add_definitions(-DTRIANGLE_HEADER="${TRIANGLE}shewchuck_triangle.h")
    set(TRIANGLE ${TRIANGLE_MESH}/triangle/ )
    add_subdirectory(${TRIANGLE})
endif()
set( Srcs
    ${TRIANGLE_MESH}/src/Vertex.cpp
    ${TRIANGLE_MESH}/src/Edge.cpp
    ${TRIANGLE_MESH}/src/Triangle.cpp
    ${TRIANGLE_MESH}/src/TriangleMesh.cpp
)
set( Hdrs
    ${TRIANGLE_MESH}/include/Vertex.h
    ${TRIANGLE_MESH}/include/Point.h
    ${TRIANGLE_MESH}/include/Edge.h
    ${TRIANGLE_MESH}/include/Triangle.h
    ${TRIANGLE_MESH}/include/TriangleMesh.h
)

add_library(${PROJECT_NAME}-lib ${Srcs} ${Hdrs})
target_include_directories(${PROJECT_NAME}-lib PRIVATE ${TRIANGLE_MESH}/include/ ${TRIANGLE})
target_link_libraries(${PROJECT_NAME}-lib Triangle-lib)
add_executable(${PROJECT_NAME}-test ${TRIANGLE_MESH}/main.cpp ${Srcs} ${Hdrs})
target_include_directories(${PROJECT_NAME}-test PRIVATE ${TRIANGLE_MESH}/include/ ${TRIANGLE})
target_link_libraries(${PROJECT_NAME}-test Triangle-lib)
