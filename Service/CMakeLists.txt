cmake_minimum_required(VERSION 3.12)

project(Elastos.MicroService.Service)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file( GLOB SOURCES "*.cpp" )

add_library(Service SHARED ${SOURCES})

target_include_directories(Service PUBLIC
                          "${PROJECT_SOURCE_DIR}"
                          "${PROJECT_SOURCE_DIR}/../Transmitter"
                          "${PROJECT_SOURCE_DIR}/../Transmitter/depends/include"
                          "${PROJECT_SOURCE_DIR}/../Transmitter/depends/include/Elastos.SDK.Contact"
                          )

find_library(SERVICE_LIBRARY Transmitter HINTS ${PROJECT_SOURCE_DIR}/../Transmitter/build)

target_link_libraries(Service PUBLIC
                      ${SERVICE_LIBRARY})
