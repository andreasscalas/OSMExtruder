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
find_path(TINYXML_H tinyxml2 )
find_library(TINYXML_LIB libtinyxml2)
find_package(PROJ)

if ( NOT GDAL_FOUND )
  message(SEND_ERROR "The GDAL library is required")
endif()

set(MAIN_FOLDER ${CMAKE_SOURCE_DIR})
set( TRIANGLE_MESH ${MAIN_FOLDER}/triangle_mesh/ )
set( TRIANGLE ${MAIN_FOLDER}/triangle/ )
set( NANOFLANN ${MAIN_FOLDER}/nanoflann/)

file(GLOB IO_srcs "${MAIN_FOLDER}/include/*.h*" "${MAIN_FOLDER}/IO/*.c*")
file(GLOB Srcs "${MAIN_FOLDER}/src/*.c*")
file(GLOB Hdrs "${MAIN_FOLDER}/include/*.h")
add_subdirectory(${TRIANGLE_MESH})
add_subdirectory(${TRIANGLE})
set(AS_LIBRARY true)

#Adding Extruder executable and dependencies
list(REMOVE_ITEM Srcs
    "${MAIN_FOLDER}/src/test_in_polygon.cpp"
    "${MAIN_FOLDER}/src/visualise_city.cpp"
    "${MAIN_FOLDER}/src/test.cpp"
    "${MAIN_FOLDER}/src/compute_slopes.cpp"
)
add_executable(${PROJECT_NAME} ${IO_srcs} ${Srcs} ${Hdrs} )
target_link_libraries(${PROJECT_NAME} ${SHAPELIB} TriangleMesh-lib Triangle-lib OpenMP::OpenMP_CXX ${GDAL_LIBRARY} ${TINYXML_LIB} ${PROJ_LIBRARIES})

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
    ${PROJ_INCLUDE_DIRS}
)

#Adding TestInPolygon executable and dependencies
file(GLOB TestInPolygonSrcs "${MAIN_FOLDER}/src/*.c*")
list(REMOVE_ITEM TestInPolygonSrcs
    "${MAIN_FOLDER}/src/main.cpp"
    "${MAIN_FOLDER}/src/visualise_city.cpp"
    "${MAIN_FOLDER}/src/test.cpp"
    "${MAIN_FOLDER}/src/compute_slopes.cpp"
)
add_executable(TestInPolygon ${IO_srcs} ${TestInPolygonSrcs} ${Hdrs} )
target_link_libraries(TestInPolygon ${SHAPELIB} TriangleMesh-lib Triangle-lib OpenMP::OpenMP_CXX ${GDAL_LIBRARY} ${TINYXML_LIB} ${PROJ_LIBRARIES})
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
    ${PROJ_INCLUDE_DIRS}
)

#Adding Test executable and dependencies
file(GLOB TestSrcs "${MAIN_FOLDER}/src/*.c*")
list(REMOVE_ITEM TestSrcs
    "${MAIN_FOLDER}/src/main.cpp"
    "${MAIN_FOLDER}/src/test_in_polygon.cpp"
    "${MAIN_FOLDER}/src/visualise_city.cpp"
    "${MAIN_FOLDER}/src/compute_slopes.cpp"
)
add_executable(Test ${IO_srcs} ${TestSrcs} ${Hdrs} )
target_link_libraries(Test ${SHAPELIB} TriangleMesh-lib Triangle-lib OpenMP::OpenMP_CXX ${GDAL_LIBRARY} ${TINYXML_LIB} ${PROJ_LIBRARIES})
target_include_directories(Test PRIVATE
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
    ${PROJ_INCLUDE_DIRS}
)

#Adding ComputeSlopes executable and dependencies
file(GLOB ComputeSlopesSrcs "${MAIN_FOLDER}/src/*.c*")
list(REMOVE_ITEM ComputeSlopesSrcs
    "${MAIN_FOLDER}/src/main.cpp"
    "${MAIN_FOLDER}/src/test.cpp"
    "${MAIN_FOLDER}/src/test_in_polygon.cpp"
    "${MAIN_FOLDER}/src/visualise_city.cpp"
)
add_executable(ComputeSlopes ${IO_srcs} ${ComputeSlopesSrcs} ${Hdrs} )
target_link_libraries(ComputeSlopes ${SHAPELIB} TriangleMesh-lib Triangle-lib OpenMP::OpenMP_CXX ${GDAL_LIBRARY} ${TINYXML_LIB} ${PROJ_LIBRARIES})
target_include_directories(ComputeSlopes PRIVATE
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
    ${PROJ_INCLUDE_DIRS}
)

#Adding VisualiseCity executable and dependencies

find_package(VTK COMPONENTS
    vtkCommonCore
    vtkFiltersCore
    vtkInfovisCore
    vtkInteractionStyle
    vtkGUISupportQt
    vtkIOLegacy
)
if(VTK_FOUND)

    include_directories(SYSTEM ${VTK_INCLUDE_DIRS})
    file(GLOB VisualiseCitySrcs "${MAIN_FOLDER}/src/*.c*")
    list(REMOVE_ITEM VisualiseCitySrcs
        "${MAIN_FOLDER}/src/main.cpp"
        "${MAIN_FOLDER}/src/test_in_polygon.cpp"
        "${MAIN_FOLDER}/src/test.cpp"
        "${MAIN_FOLDER}/src/compute_slopes.cpp"
    )
    add_executable(VisualiseCity ${IO_srcs} ${VisualiseCitySrcs} ${Hdrs} )
    target_link_libraries(VisualiseCity ${SHAPELIB} TriangleMesh-lib Triangle-lib OpenMP::OpenMP_CXX ${GDAL_LIBRARY} ${TINYXML_LIB} ${PROJ_LIBRARIES} ${VTK_LIBRARIES} )
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
        ${TINYXML_H}
        ${PROJ_INCLUDE_DIRS}
    )
endif()
