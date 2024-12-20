#!/bin/sh

. ./scripts/config.sh

printf "[ Constructing build directory. ]\n"
if [ -e build ]
then
	rm -r build
fi
mkdir build

# Make the ISO build folder structure
printf "[ Constructing grub directory. ]\n"
mkdir -p build/boot/grub

# Run Makefile 
printf "[ Compiling... ]\n"
. ./scripts/compile.sh

# Construct the grub config file
printf "[ Constructing grub.cfg. ]\n"
cat > build/boot/grub/grub.cfg << EOF
menuentry "PengOS" {
	multiboot /boot/${KERNEL}
}
EOF

# Run grub
printf "[ Building ISO... ]\n"
grub-mkrescue -o build/${ISO} build
printf "[ Running QEMU-i386. ]\n"
qemu-system-i386 -cdrom build/${ISO}