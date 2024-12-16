include ../scripts/config.sh

# Flags
C_FLAGS	?= -O2 -g
C_FLAGS	:= $(C_FLAGS) -ffreestanding

kernel:
	BOOT_OBJ = ../build/boot.o
	KERNEL_OBJ = ../build/kernel.o

# Bootstrap assembly
	i686-elf-as boot.s -o $(BOOT_OBJ)

# Compile the kernel
	i686-elf-gcc -c kernel.c -o $(KERNEL_OBJ) -std=$(C_VERSION) $(C_FLAGS) -Wall -Wextra

# Link the kernel and all source files
	i686-elf-gcc -T ../linker.ld -o ../build/boot/PengOS.bin $(C_FLAGS) -nostdlib $(BOOT_OBJ) $(KERNEL_OBJ) -lgcc