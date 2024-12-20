; Constants for multiboot header
MB_ALIGN		equ  1<<0             						; Align loaded modules on page boundaries.
MB_MEMORY_INFO	equ  1<<1             						; Provide memory map.
MB_FLAGS		equ  MB_ALIGN | MB_MEMORY_INFO				; Multiboot flags.
MB_MAGIC		equ  0x1BADB002       						; Multiboot MAGIC.
MB_CHECKSUM		equ  -(MB_MAGIC + MB_FLAGS) 				; Checksum of the above.

section .multiboot											; Defines the multiboot header.
align 4
	dd MB_MAGIC
	dd MB_FLAGS
	dd MB_CHECKSUM

section .bss												; Setup the stack.
align 16
stack_bottom:
resb 16384													; Allocate 16KB for the stack.
stack_top:

section .text
global main:function (main.end - main)
main:
	mov esp, stack_top										; Set the esp register to the top of our stack.
	extern kernel_main										; Indicate we are calling an external C function.
	call kernel_main										; Call 'kernel_main' in C.
	cli														; Disable interrupts
.hang: hlt													; Wait until the next 'hlt' instruction
	jmp .hang												; Jump to the 'hlt' instruction (infinite loop).
.end: