# #!/bin/bash

. ./scripts/config.sh

INCLUDE_ARGS=$(build_include_args)
OBJ_FILES=()

assemble_source_file() {
    debug "Assembling '$1'..."
    BASE_FILENAME=$(basename $1 | sed 's/\.[^.]*$//')
    OUT_FILENAME="${BUILD_DIR}/${BASE_FILENAME}.o"
    $AS $ASFLAGS $INCLUDE_ARGS $1 -o $OUT_FILENAME
    verify_file $OUT_FILENAME
    OBJ_FILES+=($OUT_FILENAME)
}

compile_source_file() {
    debug "Compiling '$1'..."
    BASE_FILENAME=$(basename $1 | sed 's/\.[^.]*$//')
    OUT_FILENAME="${BUILD_DIR}/${BASE_FILENAME}_c.o"
    $GCC -std=$CPP_VERSION $CFLAGS -nostdlib -fno-builtin -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -Wno-parentheses $INCLUDE_ARGS -c -o $OUT_FILENAME $1
    verify_file $OUT_FILENAME
    OBJ_FILES+=($OUT_FILENAME)
}

# Ensure build directories exist
mkdir -p $BUILD_DIR
mkdir -p $BOOT_DIR

# Bootstrap assembly
info "Assembling NASM source code..."
assemble_source_file "${SOURCE_DIR}/boot.s"
assemble_source_file "${SOURCE_DIR}/kernel/crti.s"
assemble_source_file "${SOURCE_DIR}/kernel/crtn.s"
assemble_source_file "${SOURCE_DIR}/kernel/gdt.s"
assemble_source_file "${SOURCE_DIR}/kernel/idt.s"

# Compile the kernel
info "Generating CPP source code..."
debug "Compiling with include directories: ${INCLUDE_ARGS}"
cpp_files=$(find "./source/" -type f -name "*.cpp" -o -name "*.c")
for cpp_file in ${cpp_files[@]}; do
    compile_source_file $cpp_file
done

# Link the kernel and all source files
info "Linking kernel..."
debug "Objects: [${OBJ_FILES[*]}]"
$GCC -T "${SOURCE_DIR}/linker.ld" -g -o "${BOOT_DIR}/${KERNEL}" $CFLAGS -nostdlib $INCLUDE_ARGS ${OBJ_FILES[*]} -lgcc 
verify_file "${BOOT_DIR}/${KERNEL}"

success "Compiled and linked kernel."
