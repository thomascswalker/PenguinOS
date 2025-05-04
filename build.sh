#!/bin/sh

# Ensure PATH is accurate
export PATH=$HOME/opt/cc_i386/bin:$PATH

. ./scripts/config.sh

# Add a flag to enable debug mode
VERBOSE=false
while getopts "v" opt; do
  case $opt in
    v)
      VERBOSE=true
      ;;
  esac
done

# Modify the debug function to respect the debug flag
debug() {
  if [ "$VERBOSE" = true ]; then
    echo "[DEBUG] $1"
  fi
}

debug "Constructing build directory"
if [ -e build ]
then
	rm -r build
fi
mkdir -p build

# Make the ISO build folder structure
debug "Constructing grub directory"
mkdir -p build/boot/grub

INCLUDE_DIRS=$(find ./source/ -type d)
INCLUDE_ARG=""
for DIR in $INCLUDE_DIRS; do
	INCLUDE_ARG+=" -I${DIR}"
done
info "Include arguments: ${INCLUDE_ARG}"

OUT_FILENAME=""
OBJ_FILES=()

assemble() 
{
    debug "Assembling '$1'"
    BASE_FILENAME=$(basename $1 | sed 's/\.[^.]*$//')
    OUT_FILENAME="$2/${BASE_FILENAME}.o"
    $AS $ASFLAGS $INCLUDE_ARG $1 -o $OUT_FILENAME
    verify_file $OUT_FILENAME
}

compile() 
{
    debug "Compiling '$1'"
    BASE_FILENAME=$(basename $1 | sed 's/\.[^.]*$//')
    OUT_FILENAME="$2/${BASE_FILENAME}_cpp.o"
    $GCC -std=$CPP_VERSION $CXXFLAGS -nostdlib -fno-builtin -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -Wno-parentheses -fno-exceptions $INCLUDE_ARG -c -o $OUT_FILENAME $1
    verify_file $OUT_FILENAME
}

# Ensure build directories exist
mkdir -p $BUILD_DIR
mkdir -p $BOOT_DIR

# Bootstrap assembly
debug "Compiling with include directories: ${INCLUDE_ARG}"
info "Assembling NASM source code"
FILES=$(find "./source/" -type f -name "*.s")
for FILE in ${FILES[@]}; do
    assemble $FILE $BUILD_DIR
	OBJ_FILES+=($OUT_FILENAME)
done

# Compile the kernel
info "Generating CPP source code"
FILES=$(find "./source/" -type f -name "*.cpp" -o -name "*.c")
for FILE in ${FILES[@]}; do
    compile $FILE $BUILD_DIR
	OBJ_FILES+=($OUT_FILENAME)
done

# Compile all .o files as a library
info "Creating library"
debug "Objects: [${OBJ_FILES[*]}]"
# r=Insert, c=Create, s=Index
$AR \
	rcs	"${BUILD_DIR}/${LIB}" \
	${OBJ_FILES[*]}
verify_file "${BUILD_DIR}/${LIB}"

# Link the kernel
info "Linking kernel"
$GCC \
	-T "${SOURCE_DIR}/arch/${ARCH}/linker.ld" \
	-g -m32 $CXXFLAGS -nostdlib -lgcc -lstdc++ $INCLUDE_ARG \
	-L${BUILD_DIR} -l:${LIB} \
	-o "${BOOT_DIR}/${KERNEL}"
verify_file "${BOOT_DIR}/${KERNEL}"
success "Linked kernel."

# Compile each built-in program
PROGRAMS=$(find ./programs/ -mindepth 1 -type d)
PROGRAM_BINS=()
for PROGRAM in ${PROGRAMS[@]}; do
	PROGRAM_NAME=$(basename $PROGRAM)

	PROGRAM_BUILD_DIR="${BUILD_DIR}/${PROGRAM_NAME}"
	info "Compiling program $PROGRAM_NAME to $PROGRAM_BUILD_DIR"
	mkdir -p $PROGRAM_BUILD_DIR

	PROGRAM_OBJ_FILES=()
	FILES=$(find "$PROGRAM" -type f -name "*.cpp" -o -name "*.c")
	for FILE in ${FILES[@]}; do
		compile $FILE $PROGRAM_BUILD_DIR
		PROGRAM_OBJ_FILES+=($OUT_FILENAME)
	done

	PROGRAM_BIN="${PROGRAM_BUILD_DIR}/${PROGRAM_NAME}.bin"
	$GCC \
		-Wall -Wextra -nostdlib -nostartfiles -static -s -Wl,--build-id=none -Os \
		-o "${PROGRAM_BIN}" \
		$PROGRAM_OBJ_FILES

	verify_file "${PROGRAM_BIN}"
	success "Linked program $PROGRAM_NAME."
	PROGRAM_BINS+=($PROGRAM_BIN)
done

debug "Program binaries: [${PROGRAM_BINS[*]}]"
for PROGRAM_BIN in ${PROGRAM_BINS[@]}; do
	PROGRAM_NAME=$(basename $PROGRAM_BIN)
	debug "Copying program $PROGRAM_NAME to ./base/bin"
	cp $PROGRAM_BIN "./base/bin/"
done

$GCC \
	-T "${SOURCE_DIR}/arch/${ARCH}/linker.ld" \
	-g -m32 $CXXFLAGS -nostdlib -lgcc $INCLUDE_ARG \
	-L${BUILD_DIR} -l:${LIB}

# Construct the grub config file
debug "Constructing grub.cfg"
cat > "build/boot/grub/grub.cfg" << EOF
menuentry "PengOS" {
	multiboot /boot/${KERNEL}
}
EOF

# Run grub
info "Building ISO"
grub-mkrescue \
	-o "${BOOT_DIR}/${ISO}" \
	build
success "ISO built at '${BOOT_DIR}/${ISO}'."

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
	-cdrom "${BOOT_DIR}/${ISO}" \
	 -boot d \
	-drive file=disk.img,format=raw,if=ide \
	-display gtk,zoom-to-fit=on \
	# -d int \
	# -no-reboot \
	# -no-shutdown \
	# -s -S