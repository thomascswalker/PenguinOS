[bits 32]

; Constants for multiboot header
MB_ALIGN		equ  1<<0             						; Align loaded modules on page boundaries.
MB_MEMORY_INFO	equ  1<<1             						; Provide memory map.
MB_GFX          equ  0                                      ; Enable GUI
MB_FLAGS		equ  MB_ALIGN | MB_MEMORY_INFO | MB_GFX		; Multiboot flags.
MB_MAGIC		equ  0x1BADB002       						; Multiboot MAGIC.
MB_CHECKSUM		equ  -(MB_MAGIC + MB_FLAGS) 				; Checksum of the above.

STACKSIZE equ 16 * 0x400                                    ; 16KB

section .multiboot											; Defines the multiboot header.
align 4
	dd MB_MAGIC
	dd MB_FLAGS
	dd MB_CHECKSUM
    dd 0, 0, 0, 0, 0

section .text
global _start
_start:
    mov esp, (stack + STACKSIZE)                            ; Move stack pointer into esp

    push eax                                                ; Multiboot header
    push ebx                                                ; Multiboot magic
    xor ebp, ebp

    extern kmain                                            ; External reference to kmain
    call kmain                                              ; Call kmain in main.c
    cli                                                     ; Disable interrupts
halt:
    hlt                                                     ; Halt the next interrupt
    jmp halt

section .bss                                                ; Setup the stack.
align 32
stack:
    resb 16384											    ; Allocate 16KB for the stack.

global esp0_stack_bottom
global esp0_stack_top
align 4096
esp0_stack_bottom:
    resb 8192
esp0_stack_top: