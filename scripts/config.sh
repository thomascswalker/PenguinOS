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

export CFLAGS="-O0 -m32 -g -ffreestanding -Wall -Wextra -fno-omit-frame-pointer"
export ASFLAGS="-felf32 -g"

export KERNEL="PengOS.bin"
export ISO="PengOS.iso"

# Colors
RED='\e[0;31m'
BLUE='\e[0;34m'
GREEN='\e[0;32m'
YELLOW='\e[0;33m'
GRAY='\e[0;37m'
NC='\e[0m' # No Color

build_include_args() {
    result=""
    dirs=$(find "./source/arch/${ARCH}" -type d)
    for dir in $dirs; do
        result+=" -I${dir}"
    done
    
    echo -e \
        "-I./source/ \
        -I./source/libc \
        -I./source/libcpp \
        -I./source/filesystem \
        -I./source/memory \
        -I./source/processes \
        -I./source/system \
        ${result# }"
}

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

verify_file() {
    if [ -f $1 ]; then
        debug "Compiled to '${1}'."
    else
        error "Failed to compile '${1}'."
    fi
}