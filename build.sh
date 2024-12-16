rm -r build
mkdir build

# Bootstrap assembly
i686-elf-as source/boot.s -o build/boot.o

# Compile the kernel
i686-elf-gcc -c source/kernel.c -o build/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

# Link the kernel and all source files
i686-elf-gcc -T linker.ld -o build/PengOS.bin -ffreestanding -O2 -nostdlib build/boot.o build/kernel.o -lgcc

# Make the ISO build folder structure
mkdir -p build/iso/boot/grub

# Move the bin file we created
mv build/PengOS.bin build/iso/boot/PengOS.bin

# Copy the grub config file
cp grub.cfg build/iso/boot/grub/grub.cfg

# Run grub
grub-mkrescue -o build/iso/PengOS.iso build/iso

qemu-system-i386 -cdrom build/iso/PengOS.iso