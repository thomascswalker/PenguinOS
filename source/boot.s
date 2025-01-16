[bits 32]

; Constants for multiboot header
MB_ALIGN		equ  1<<0             						; Align loaded modules on page boundaries.
MB_MEMORY_INFO	equ  1<<1             						; Provide memory map.
MB_GFX          equ  0                                      ; Enable GUI
MB_FLAGS		equ  MB_ALIGN | MB_MEMORY_INFO | MB_GFX		; Multiboot flags.
MB_MAGIC		equ  0x1BADB002       						; Multiboot MAGIC.
MB_CHECKSUM		equ  -(MB_MAGIC + MB_FLAGS) 				; Checksum of the above.

KERNEL_VIRTUAL_BASE equ 0xC0000000                          ; 3GB
KERNEL_PAGE_COUNT equ (KERNEL_VIRTUAL_BASE >> 22)           ; 768

STACKSIZE equ 16 * 0x400                                    ; 16KB

section .multiboot											; Defines the multiboot header.
align 4
	dd MB_MAGIC
	dd MB_FLAGS
	dd MB_CHECKSUM
    dd 0, 0, 0, 0, 0

align 0x1000
bootPageDirectory:
    dd 0x00000083
    times (KERNEL_PAGE_COUNT - 1) dd 0
    ; This page directory entry defines a 4MB page containing the kernel.
    dd 0x00000083
    times (1024 - KERNEL_PAGE_COUNT - 1) dd 0

global _start
_start:
    mov ecx, bootPageDirectory
    mov cr3, ecx                                            ; Move into control register CR3, tells the processor where 
                                                            ; the location of the page directory and page tables is.
    mov ecx, cr4
    or ecx, 0x10                                            ; Set physical address extension
    mov cr4, ecx

    mov ecx, cr0
    or ecx, 0x80000000                                      ; Enables paging on our system
    mov cr0, ecx

    jmp higherHalf

section .text
higherHalf:
    ; mov dword [bootPageDirectory], 0
    ; invlpg [0]

    mov esp, (stack + STACKSIZE)                            ; Move stack pointer into esp
    push eax
    add ebx, KERNEL_VIRTUAL_BASE
    push ebx                                                ; Push ebx onto the stack

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
