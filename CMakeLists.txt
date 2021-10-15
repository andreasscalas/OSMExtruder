cmake_minimum_required(VERSION 3.5)

project(Extruder LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
find_library(SHAPELIB shp Shp)
find_path(SHAPELIB_H shapelib)
set(MAIN_FOLDER ${CMAKE_SOURCE_DIR})
set( IMATISTL ${MAIN_FOLDER}/ImatiSTL)
set( IMATISTL_H ${IMATISTL}/include)


file(GLOB IO_srcs "${MAIN_FOLDER}/include/*.h*" "${MAIN_FOLDER}/IO/*.c*")
file(GLOB Srcs "${MAIN_FOLDER}/src/*.c*")

add_subdirectory(${IMATISTL})
add_executable(${PROJECT_NAME} ${IO_srcs} ${Srcs} )
target_link_libraries(${PROJECT_NAME} ${SHAPELIB} ImatiSTL-lib)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${MAIN_FOLDER}/include/
    ${SHAPELIB_H}
    ${IMATISTL_H}/
    ${IMATISTL_H}/ImatiSTL/
    ${IMATISTL_H}/Kernel/
    ${IMATISTL_H}/TMesh/
)


