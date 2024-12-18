include ../scripts/config.sh

# Flags
C_FLAGS	?= -O2 -g
C_FLAGS	:= $(C_FLAGS) -ffreestanding -Wall -Wextra

# Include directories
INCLUDE_DIRS ?= stdlib kernel

main:
# Bootstrap assembly
	@printf "[ Bootstrapping... ]\n"
	$(AS) boot.s -o ../build/boot.o

# Compile the kernel
	@printf "[ Generating kernel symbols... ]\n"
	$(GCC) -std=$(C_VERSION) $(C_FLAGS) -nostdlib -fno-builtin $(foreach dir, $(INCLUDE_DIRS), -I./$(dir)) -c -o ../build/main.o main.c

# Link the kernel and all source files
	@printf "[ Linking kernel... ]\n"
	$(GCC) -T linker.ld -o ../build/boot/PengOS.bin $(C_FLAGS) -nostdlib ../build/boot.o ../build/main.o

	@printf "[ Makefile complete. ]\n"