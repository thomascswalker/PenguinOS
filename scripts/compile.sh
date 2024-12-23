# #!/bin/bash

. ./scripts/config.sh

INCLUDE_DIRS=$(build_include_args)
OBJ_FILES=()

assemble_source_file() {
    debug "Assembling '$1'..."
    BASE_FILENAME=$(basename $1 | sed 's/\.[^.]*$//')
    OUT_FILENAME="${BUILD_DIR}/${BASE_FILENAME}.o"
    $AS $ASFLAGS $INCLUDE_DIRS $1 -o $OUT_FILENAME
    verify_file $OUT_FILENAME
    OBJ_FILES+=($OUT_FILENAME)
}

compile_source_file() {
    debug "Compiling '$1'..."
    BASE_FILENAME=$(basename $1 | sed 's/\.[^.]*$//')
    OUT_FILENAME="${BUILD_DIR}/${BASE_FILENAME}.o"
    $GCC -std=$C_VERSION $CFLAGS -nostdlib -fno-builtin $INCLUDE_DIRS -c -o $OUT_FILENAME $1
    verify_file $OUT_FILENAME
    OBJ_FILES+=($OUT_FILENAME)
}

# Ensure build directories exist
mkdir -p $BUILD_DIR
mkdir -p $BOOT_DIR

# Bootstrap assembly
info "Bootstrapping..."
assemble_source_file "${SOURCE_DIR}/boot.s"
assemble_source_file "${SOURCE_DIR}/kernel/crti.s"
assemble_source_file "${SOURCE_DIR}/kernel/crtn.s"
assemble_source_file "${SOURCE_DIR}/kernel/gdt.s"
assemble_source_file "${SOURCE_DIR}/kernel/idt.s"

# Compile the kernel
info "Generating kernel symbols..."
debug "Compiling with include directories: ${INCLUDE_DIRS}"
compile_source_file "${SOURCE_DIR}/main.c"

# Link the kernel and all source files
info "Linking kernel..."
debug "Objects: [${OBJ_FILES[*]}]"
$GCC -T "${SOURCE_DIR}/linker.ld" -g -o "${BOOT_DIR}/${KERNEL}" $CFLAGS -nostdlib $INCLUDE_DIRS -lgcc ${OBJ_FILES[*]}
verify_file "${BOOT_DIR}/${KERNEL}"

success "Compiled and linked kernel."
