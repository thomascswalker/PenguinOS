#!/bin/bash

export C_VERSION=c23

export MAKE=${MAKE:-make}

export HOST=i686-elf
export GCC=${HOST}-gcc
export AS=nasm

INCLUDE_DIRS=("kernel" "stdlib")
build_include_args() {
    result=""
    for dir in "${INCLUDE_DIRS[@]}"; do
        result+=" -I./source/${dir}"
    done
    
    echo "-I.source/ ${result# }"
}

export SOURCE_DIR="./source"
export BUILD_DIR="./build"
export BOOT_DIR="$BUILD_DIR/boot"

export CFLAGS="-O2 -g -ffreestanding -Wall -Wextra"
export ASFLAGS="-f elf32"

export KERNEL="PengOS.bin"
export ISO="PengOS.iso"

error_exit() {
    printf "ERROR: $1\n"
    exit 1
}

verify_file() {
    if [ -f $1 ]; then
        printf "Compiled to '$1'.\n"
    else
        error_exit "Failed to compile '$1'."
    fi
}