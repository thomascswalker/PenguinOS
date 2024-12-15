rm -r build
mkdir build

# Bootstrap assembly
i686-elf-as source/boot.s -o build/boot.o

# Compile the kernel
i686-elf-gcc -c source/kernel.c -o build/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

# Link the kernel
i686-elf-gcc -T source/linker.ld -o build/myos.bin -ffreestanding -O2 -nostdlib build/boot.o build/kernel.o -lgcc

# Make the ISO build folder structure
mkdir -p build/isodir/boot/grub

# Move the bin file we created
mv build/myos.bin build/isodir/boot/myos.bin

# Copy the grub config file
cp source/grub.cfg build/isodir/boot/grub/grub.cfg

# Run grub
grub-mkrescue -o build/isodir/myos.iso build/isodir

qemu-system-i386 -cdrom build/isodir/myos.iso