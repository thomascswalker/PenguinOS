[bits 32]

; Constants for multiboot header
MB_ALIGN		equ  1<<0             						; Align loaded modules on page boundaries.
MB_MEMORY_INFO	equ  1<<1             						; Provide memory map.
MB_GFX          equ  0                                      ; Enable GUI
MB_FLAGS		equ  MB_ALIGN | MB_MEMORY_INFO | MB_GFX		; Multiboot flags.
MB_MAGIC		equ  0x1BADB002       						; Multiboot MAGIC.
MB_CHECKSUM		equ  -(MB_MAGIC + MB_FLAGS) 				; Checksum of the above.

section .multiboot											; Defines the multiboot header.
align 4
	dd MB_MAGIC
	dd MB_FLAGS
	dd MB_CHECKSUM
    dd 0, 0, 0, 0, 0

    ; Graphics
    dd 0                                                    ; linear graphic mode
    dd 800                                                  ; width
    dd 600                                                  ; height
    dd 32                                                   ; bits per pixel

section .bss                                                ; Setup the stack.
align 16
stack_bottom:
    resb 16384 * 8											; Allocate 131KB for the stack.
stack_top:

section .boot
global _start
_start:
    ; Enable A20 line
    in al, 0x92
	or al, 2
	out 0x92, al

    mov eax, (initial_page_dir - 0xC0000000)
    mov cr3, eax                                            ; Move into control register CR3, tells the processor where 
                                                            ; the location of the page directory and page tables is.
    mov ecx, cr4
    or ecx, 0x10                                            ; Set physical address extension
    mov cr4, ecx

    mov ecx, cr0
    or ecx, 0x80000000                                      ; Enables paging on our system
    mov cr0, ecx

    jmp higher_half

section .text
higher_half:
    mov esp, stack_top                                      ; Move stack pointer into esp
    push ebx                                                ; Push ebx onto the stack
    xor ebp, ebp                                            ; Reset ebp

    extern kernel_main                                      ; External reference to kernel_main
    call kernel_main                                        ; Call kernel_main in main.c
    cli                                                     ; Disable interrupts
    hlt                                                     ; Halt the next interrupt
halt:
    jmp halt

; Initialize paging table
section .data
align 4096
global initial_page_dir
initial_page_dir:
    dd 10000011b
    times 768-1 dd 0

    dd (0 << 22) | 10000011b
    dd (1 << 22) | 10000011b
    dd (2 << 22) | 10000011b
    dd (3 << 22) | 10000011b
    times 256-4 dd 0