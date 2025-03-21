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

export PATH=$HOME/opt/cc_i386/bin:$PATH

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
grub-mkrescue \
	-o build/${ISO} \
	build
success "ISO built at './build/${ISO}'."

info "Building disk image..."
if [ -e ./disk.img ]
then
	rm ./disk.img
fi
. ./build_disk_image.sh
success "Disk image built."

# Run QEMU
info "Running QEMU-i386."
MEM_SIZE=32 # Memory size in megabytes
qemu-system-i386 \
	-m ${MEM_SIZE}M \
	-cdrom ./build/${ISO} \
	 -boot d \
	-drive file=disk.img,format=raw,if=ide \
	-display gtk,zoom-to-fit=on \
	-d int \
	-no-reboot \
	-no-shutdown \
	# -s -S