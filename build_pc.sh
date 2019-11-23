#!/bin/bash

set -e
set -u

OUTPUT_PATH=${OUTPUT_HOME:-}
if [ -z ${OUTPUT_PATH} ]; then
	OUTPUT_PATH="$(pwd)/output"
fi
echo "build path: ${OUTPUT_PATH}"
mkdir -p  ${OUTPUT_PATH}/build
mkdir -p  ${OUTPUT_PATH}/dict

CURRENT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
PROJECT_DIR=$(dirname "$CURRENT_DIR")
SDK_DIR="${PROJECT_DIR}/Elastos.SDK.ElephantWallet.Contact"
SDK_INSTALL_DIR="${SDK_DIR}/build/sysroot/Darwin/x86_64"
SDK_INCLUDE_DIR="${SDK_INSTALL_DIR}/include/"

# copy Josn.hpp to install path
if [ ! -e ${SDK_INCLUDE_DIR}/include/Json.hpp ]; then
	echo "Copy SDK include to ${SDK_INCLUDE_DIR}"
	#mkdir -p ${SDK_INCLUDE_DIR}
	cp -r ${SDK_DIR}/lib/wrapper/json/*.hpp ${SDK_INCLUDE_DIR}
fi


loginfo() {
	echo -e "\033[1;34m [-]: $@ \033[00m";
}

build_module() {
	loginfo "To build module: ${1}"

	src_dir="$(pwd)/${1}"
	mkdir -p ${OUTPUT_PATH}/build/${1}
	pushd ${OUTPUT_PATH}/build/${1}
	cmake -DCMAKE_INSTALL_PREFIX=${OUTPUT_PATH}/dict ${2} ${src_dir}
	make && make install
	popd
}

build_module Launcher ""
build_module Transmitter "-DSDK_INCLUDE_DIR=${SDK_INCLUDE_DIR} -DSDK_INSTALL_PREFIX=${SDK_INSTALL_DIR}"
