cmake_minimum_required(VERSION 3.5)

project(Extruder LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
find_package(OpenMP)
find_library(SHAPELIB shp Shp)
find_path(SHAPELIB_H shapelib)
find_package( GDAL )
find_package(RapidJSON)
find_path(RAPIDJSON_H rapidjson-master/include/)

if ( NOT GDAL_FOUND )
  message(SEND_ERROR "The GDAL library is required")
endif()

set(MAIN_FOLDER ${CMAKE_SOURCE_DIR})
set( TRIANGLE_MESH ${MAIN_FOLDER}/triangle_mesh/ )
set( TRIANGLE ${MAIN_FOLDER}/triangle/ )
set( NANOFLANN ${MAIN_FOLDER}/nanoflann/)

add_definitions(-DTRIANGLE_HEADER="${TRIANGLE}shewchuck_triangle.h")

file(GLOB IO_srcs "${MAIN_FOLDER}/include/*.h*" "${MAIN_FOLDER}/IO/*.c*")
file(GLOB Srcs "${MAIN_FOLDER}/src/*.c*")
file(GLOB TestInPolygonSrcs "${MAIN_FOLDER}/src/*.c*")
file(GLOB VisualiseCitySrcs "${MAIN_FOLDER}/src/*.c*")
list(REMOVE_ITEM Srcs
    "${MAIN_FOLDER}/src/testInPolygon.cpp"
    "${MAIN_FOLDER}/src/visualiseCity.cpp"
)
list(REMOVE_ITEM TestInPolygonSrcs
    "${MAIN_FOLDER}/src/main.cpp"
    "${MAIN_FOLDER}/src/visualiseCity.cpp"
)
list(REMOVE_ITEM VisualiseCitySrcs
    "${MAIN_FOLDER}/src/main.cpp"
    "${MAIN_FOLDER}/src/testInPolygon.cpp"
)
add_subdirectory(${TRIANGLE_MESH})
add_subdirectory(${TRIANGLE})
set(AS_LIBRARY true)
add_executable(${PROJECT_NAME} ${IO_srcs} ${Srcs} )
add_executable(TestInPolygon ${IO_srcs} ${TestInPolygonSrcs} )
target_link_libraries(${PROJECT_NAME} ${SHAPELIB} TriangleMesh-lib Triangle-lib OpenMP::OpenMP_CXX ${GDAL_LIBRARY} )
target_link_libraries(TestInPolygon ${SHAPELIB} TriangleMesh-lib Triangle-lib OpenMP::OpenMP_CXX ${GDAL_LIBRARY} )

target_include_directories(${PROJECT_NAME} PRIVATE
    ${MAIN_FOLDER}/include/
    ${MAIN_FOLDER}/prepair/
    ${SHAPELIB_H}
    ${GDAL_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
    ${NANOFLANN}/include/
    ${NANOFLANN}/examples/
    ${TRIANGLE}
    ${TRIANGLE_MESH}/include
    ${RAPIDJSON_H}
)

target_include_directories(TestInPolygon PRIVATE
    ${MAIN_FOLDER}/include/
    ${MAIN_FOLDER}/prepair/
    ${SHAPELIB_H}
    ${GDAL_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
    ${NANOFLANN}/include/
    ${NANOFLANN}/examples/
    ${TRIANGLE}
    ${TRIANGLE_MESH}/include
    ${RAPIDJSON_H}
)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonCore
    vtkFiltersCore
    vtkInfovisCore
    vtkInteractionStyle
    vtkViewsQt
    vtkGUISupportQt
    vtkIOLegacy
)
if(VTK_FOUND)

    include(${VTK_USE_FILE})

    add_executable(VisualiseCity ${IO_srcs} ${VisualiseCitySrcs} )
    target_link_libraries(VisualiseCity ${SHAPELIB} TriangleMesh-lib Triangle-lib OpenMP::OpenMP_CXX ${GDAL_LIBRARY} ${VTK_LIBRARIES} )
    target_include_directories(VisualiseCity PRIVATE
        ${MAIN_FOLDER}/include/
        ${MAIN_FOLDER}/prepair/
        ${SHAPELIB_H}
        ${GDAL_INCLUDE_DIR}
        ${Boost_INCLUDE_DIR}
        ${NANOFLANN}/include/
        ${NANOFLANN}/examples/
        ${TRIANGLE}
        ${TRIANGLE_MESH}/include
        ${RAPIDJSON_H}
    )
endif()
