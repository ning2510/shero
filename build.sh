#!/bin/bash

set -x

SOURCE_DIR=`pwd`
BUILD_DIR=${BUILD_DIR:-./build}
BIN_DIR=${BIN_DIR:-./bin}
LIB_DIR=${LIB_DIR:-./lib}
BENCH_MARK=${BENCH_MARK:-OFF}
BUILD_TYPE=${BUILD_TYPE:-Debug}

PATH_INSTALL_INC_ROOT=${PATH_INSTALL_INC_ROOT:-/usr/local/include}
PATH_INSTALL_LIB_ROOT=${PATH_INSTALL_LIB_ROOT:-/usr/local/lib}
INCLUDE_DIR=${INCLUDE_DIR:-./include}
LIB=${LIB:-./lib/libshero.a}

if [ "$1" == "release" ]; then
    BENCH_MARK=ON
    BUILD_TYPE=Relaese
fi

mkdir -p ${BUILD_DIR} ${BIN_DIR} ${LIB_DIR} ${INCLUDE_DIR} \
    && cd ${BUILD_DIR} \
    && cmake \
        -DBENCH_MARK=$BENCH_MARK \
        -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
        .. \
    && make install -j$(nproc)
