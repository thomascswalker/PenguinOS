#!/bin/sh

. ./scripts/config.sh

debug "Constructing build directory."
if [ -e build ]
then
	rm -r build
fi
mkdir build

# Make the ISO build folder structure
debug "Constructing grub directory."
mkdir -p build/boot/grub

# Run Makefile 
info "Compiling..."
. ./scripts/compile.sh

# Construct the grub config file
debug "Constructing grub.cfg..."
cat > build/boot/grub/grub.cfg << EOF
menuentry "PengOS" {
	multiboot /boot/${KERNEL}
}
EOF

# Run grub
info "Building ISO..."
grub-mkrescue -o build/${ISO} --rom-directory=build/${ISO} build
success "ISO built at './build/${ISO}'."

# Run QEMU
info "Running QEMU-i386."
setsid qemu-system-i386 -cdrom build/${ISO} -m 512M -display gtk,zoom-to-fit=on # -S -gdb tcp::1234
