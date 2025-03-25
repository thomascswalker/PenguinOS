#!/bin/sh

. ./scripts/config.sh

debug "Constructing build directory"
if [ -e build ]
then
	rm -r build
fi
mkdir build

# Make the ISO build folder structure
debug "Constructing grub directory"
mkdir -p build/boot/grub

export PATH=$HOME/opt/cc_i386/bin:$PATH

INCLUDE_ARGS=$(build_include_args)
OBJ_FILES=()

assemble_source_file() {
    debug "Assembling '$1'"
    BASE_FILENAME=$(basename $1 | sed 's/\.[^.]*$//')
    OUT_FILENAME="${BUILD_DIR}/${BASE_FILENAME}.o"
    $AS $ASFLAGS $INCLUDE_ARGS $1 -o $OUT_FILENAME
    verify_file $OUT_FILENAME
    OBJ_FILES+=($OUT_FILENAME)
}

compile_source_file() {
    debug "Compiling '$1'"
    BASE_FILENAME=$(basename $1 | sed 's/\.[^.]*$//')
    OUT_FILENAME="${BUILD_DIR}/${BASE_FILENAME}_cpp.o"
    $GCC -std=$CPP_VERSION $CFLAGS -nostdlib -fno-builtin -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -Wno-parentheses -fno-exceptions $INCLUDE_ARGS -c -o $OUT_FILENAME $1
    verify_file $OUT_FILENAME
    OBJ_FILES+=($OUT_FILENAME)
}

# Ensure build directories exist
mkdir -p $BUILD_DIR
mkdir -p $BOOT_DIR

# Bootstrap assembly
debug "Compiling with include directories: ${INCLUDE_ARGS}"
info "Assembling NASM source code"
cpp_files=$(find "./source/" -type f -name "*.s")
for cpp_file in ${cpp_files[@]}; do
    assemble_source_file $cpp_file
done

# Compile the kernel
info "Generating CPP source code"
cpp_files=$(find "./source/" -type f -name "*.cpp" -o -name "*.c")
for cpp_file in ${cpp_files[@]}; do
    compile_source_file $cpp_file
done

# Compile all .o files as a library
info "Linking kernel"
debug "Objects: [${OBJ_FILES[*]}]"
# r=Insert, c=Create, s=Index
LIB_FILE="PengOS.a"
$AR \
	rcs	"${BUILD_DIR}/${LIB_FILE}" \
	${OBJ_FILES[*]}

for obj_file in ${OBJ_FILES[@]}; do
	rm $obj_file
done

# Link the kernel
$GCC \
	-T "${SOURCE_DIR}/arch/${ARCH}/linker.ld" \
	-g -m32 $CFLAGS -nostdlib -lgcc $INCLUDE_ARGS \
	-L${BUILD_DIR} -l:${LIB_FILE} \
	-o "${BOOT_DIR}/${KERNEL}"
verify_file "${BOOT_DIR}/${KERNEL}"
success "Linked kernel."

# Construct the grub config file
debug "Constructing grub.cfg"
cat > build/boot/grub/grub.cfg << EOF
menuentry "PengOS" {
	multiboot /boot/${KERNEL}
}
EOF

# Run grub
info "Building ISO"
grub-mkrescue \
	-o build/${ISO} \
	build
success "ISO built at './build/${ISO}'."

info "Building disk image"
if [ -e ./disk.img ]
then
	rm ./disk.img
fi
. ./build_disk_image.sh
success "Disk image built."

# Run QEMU
info "Running QEMU-i386"
MEM_SIZE=32 # Memory size in megabytes
qemu-system-i386 \
	-m ${MEM_SIZE}M \
	-cdrom ./build/${ISO} \
	 -boot d \
	-drive file=disk.img,format=raw,if=ide \
	-display gtk,zoom-to-fit=on \
	# -d int \
	# -no-reboot \
	# -no-shutdown \
	# -s -S