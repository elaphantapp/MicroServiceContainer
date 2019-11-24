cmake_minimum_required(VERSION 3.12)

project(Elastos.MicroService.Launcher)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)


file( GLOB SOURCES "*.cpp" )

add_executable(Launcher ${SOURCES})


find_package(PkgConfig REQUIRED)
set(ENV{PKG_CONFIG_PATH}  "${SDK_INSTALL_PREFIX}/lib/pkgconfig")
message(STATUS "----${SDK_INSTALL_PREFIX}/lib/pkgconfig")
pkg_check_modules(SDK_KEYPAIR REQUIRED IMPORTED_TARGET Elastos.SDK.Keypair.C)

# to fine openssl library
pkg_check_modules(OPENSSL REQUIRED IMPORTED_TARGET openssl)

target_include_directories(Launcher PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/../Service"
                          ${SDK_KEYPAIR_INCLUDE_DIRS}
                          )

target_link_libraries(Launcher PUBLIC
		    ${SDK_KEYPAIR_LDFLAGS}
		    ${OPENSSL_LIBRARIES})

INSTALL(TARGETS Launcher DESTINATION bin)
