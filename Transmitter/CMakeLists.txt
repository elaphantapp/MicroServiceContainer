cmake_minimum_required(VERSION 3.12)

project(Elastos.MicroService.Transmitter)

# include cpp files not static library
file( GLOB SERVICE_SOURCES "../Service*.cpp" )

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file( GLOB TRANSMITTER_SOURCES "*.cpp" )


find_package(PkgConfig REQUIRED)
set(ENV{PKG_CONFIG_PATH}  "${SDK_INSTALL_PREFIX}/lib/pkgconfig")
pkg_check_modules(SDK_KEYPAIR REQUIRED IMPORTED_TARGET Elastos.SDK.Keypair.C)
message(STATUS "======Found SDK.Keypair includ: ${SDK_KEYPAIR_INCLUDE_DIRS}")

pkg_check_modules(SDK_CONTACT REQUIRED IMPORTED_TARGET Elastos.SDK.Contact)
message(STATUS "======Found SDK.Contract includ: ${SDK_CONTACT_INCLUDE_DIRS}")

include_directories(${SDK_INCLUDE_DIR} ${SDK_KEYPAIR_INCLUDE_DIRS} ${SDK_CONTACT_INCLUDE_DIRS} ../Service)

add_library(TransmitterLib MODULE ${TRANSMITTER_SOURCES} ${SERVICE_SOURCES} )

target_link_libraries(TransmitterLib PUBLIC ${SDK_KEYPAIR_LDFLAGS} ${SDK_CONTACT_LDFLAGS})

install(TARGETS TransmitterLib DESTINATION lib)
