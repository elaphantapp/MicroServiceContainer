cmake_minimum_required(VERSION 3.12)

project(Elastos.MicroService.Transmitter)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file( GLOB SOURCES "*.cpp" )

add_library(Transmitter SHARED ${SOURCES})

target_include_directories(Transmitter PUBLIC
                          "${PROJECT_SOURCE_DIR}/../Service"
                          "${PROJECT_SOURCE_DIR}/depends/include"
                          "${PROJECT_SOURCE_DIR}/depends/include/Elastos.SDK.Contact"
                          )

find_library(LIBRARY Elastos.SDK.Contact HINTS ${PROJECT_SOURCE_DIR}/depends/lib)

target_link_libraries(Transmitter PUBLIC
                      ${LIBRARY})
