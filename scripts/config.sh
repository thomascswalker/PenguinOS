#!/bin/bash

export C_VERSION=c23
export CPP_VERSION=c++23

export MAKE=${MAKE:-make}

export ARCH=i386
export HOST=i686-elf
export GCC=${HOST}-g++
export AR=${HOST}-ar
export LD=${HOST}-ld
export AS=nasm

export SOURCE_DIR="./source"
export BUILD_DIR="./build"
export BOOT_DIR="$BUILD_DIR/boot"

export CXXFLAGS="-O0 -m32 -g -Wall -ffreestanding -fno-omit-frame-pointer -fno-rtti"
export ASFLAGS="-felf32 -g"

export LIB="PengOS.a"
export KERNEL="PengOS.bin"
export ISO="PengOS.iso"

# Colors
RED='\e[0;31m'
BLUE='\e[0;34m'
GREEN='\e[0;32m'
YELLOW='\e[0;33m'
GRAY='\e[0;37m'
NC='\e[0m' # No Color

debug() {
    echo -e "${GRAY}[ DEBUG   ] ${1}${NC}"
}

info() {
    echo -e "[ INFO    ] ${1}"
}

error() {
    echo -e "${RED}[ ERROR   ] ${1}${NC}"
    exit 1
}

warning() {
    echo -e "${YELLOW}[ WARNING ] ${1}${NC}"
}

success() {
    echo -e "${GREEN}[ SUCCESS ] ${1}${NC}"
}


get_include_dirs()
{
    # debug "Finding include directories..."

    RESULT=()
    DIRS=$(find "./source" -type d)
    for DIR in $DIRS; do
        # debug "DIR: ${DIR}"
        RESULT+=($DIR)
    done

    echo "${RESULT# }"
}

get_lib_dirs()
{
    echo "./source/libc ./source/libcpp"
}

verify_file() {
    if [ -f $1 ]; then
        debug "Compiled to '${1}'."
    else
        error "Failed to compile '${1}'."
        exit
    fi
}