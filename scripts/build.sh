#!/bin/sh

. ./scripts/config.sh

rm -r build
mkdir build

# Make the ISO build folder structure
mkdir -p build/boot/grub

# Run Makefile 
(cd source && $MAKE kernel)

# Construct the grub config file
cat > build/boot/grub/grub.cfg << EOF
menuentry "PengOS" {
	multiboot /boot/PengOS.bin
}
EOF

# Run grub
grub-mkrescue -o build/PengOS.iso build

qemu-system-i386 -cdrom build/PengOS.iso