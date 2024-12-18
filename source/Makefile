include ../scripts/config.sh

# Flags
C_FLAGS	?= -O2 -g
C_FLAGS	:= $(C_FLAGS) -ffreestanding -Wall -Wextra

# Include directories
INCLUDE_DIRS ?= stdlib kernel

kernel:
# Bootstrap assembly
	$(AS) boot.s -o ../build/boot.o

# Compile the kernel
	$(GCC) -c kernel.c -o ../build/kernel.o -std=$(C_VERSION) $(C_FLAGS) -nostdlib -I $(foreach dir, $(INCLUDE_DIRS), -I$(dir))

# # Compile other source files
# 	$(GCC) -c stdlib/terminal.h -o ../build/terminal.o -std=$(C_VERSION) $(C_FLAGS) -Wall -Wextra

# Link the kernel and all source files
	$(GCC) -T linker.ld -o ../build/boot/PengOS.bin $(C_FLAGS) -nostdlib ../build/boot.o ../build/kernel.o