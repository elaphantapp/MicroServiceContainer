cmake_minimum_required(VERSION 3.12)

project(Elastos.MicroService.Launcher)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)


file( GLOB SOURCES "*.cpp" )

add_executable(Launcher ${SOURCES})

target_include_directories(Launcher PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/../Service"
                          "${PROJECT_SOURCE_DIR}/depends"
                          )

find_library(LAUNCHER_LIBRARY Elastos.SDK.Keypair.C HINTS ${PROJECT_SOURCE_DIR}/depends)

target_link_libraries(Launcher PUBLIC
                      ${LAUNCHER_LIBRARY})
