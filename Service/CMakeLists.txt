cmake_minimum_required(VERSION 3.12)

project(Elastos.MicroService.Service)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file( GLOB SERVICE_SOURCES "*.cpp" )

find_package(PkgConfig REQUIRED)
set(ENV{PKG_CONFIG_PATH}  "${SDK_INSTALL_PREFIX}/lib/pkgconfig")

pkg_check_modules(SDK_CONTACT REQUIRED IMPORTED_TARGET Elastos.SDK.Contact)
message(STATUS "======Found SDK.Contract includ: ${SDK_CONTACT_INCLUDE_DIRS}")

find_library(TRANSMITTER_LIB transmitter)

include_directories(${SDK_INCLUDE_DIR} ${SDK_CONTACT_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/../Transmitter/)

add_library(service SHARED ${SERVICE_SOURCES})

target_link_libraries(service ${TRANSMITTER_LIB})

install(TARGETS service DESTINATION lib)
