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

; Initialize paging table
section .data
align 4096
boot_page_directory:
; 4MB lower-half kernel page
    dd 0x83
    times (KERNEL_PAGE_COUNT - 1) dd 0

    dd 0x83
    times (1024 - KERNEL_PAGE_COUNT - 1) dd 0

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

section .text
global _start:function (_start.end - _start)
_start:
    ; Enable A20 line
    in al, 0x92
	or al, 2
	out 0x92, al

    mov ecx, (boot_page_directory - KERNEL_VIRTUAL_BASE)
    mov cr3, ecx                                            ; Move into control register CR3, tells the processor where 
                                                            ; the location of the page directory and page tables is.
    mov ecx, cr4
    or ecx, (1 << 4)                                        ; Set PSE bit in CR4 to enable 4MB pages.
    mov cr4, ecx

    mov ecx, cr0
    or ecx, (1 << 31)                                       ; Set PG bit in CR0 to enable pageing.
    mov cr0, ecx

    lea ecx, [.higher_half]                                 ; Far jump
    jmp ecx
.higher_half:
    ; Unmap lower-half kernel page
    mov dword [boot_page_directory], 0
    invlpg [0]

    ; Setup stack
    mov esp, stack_top                                      ; Move stack pointer into esp

    ; Multiboot
    push ebx                                                ; multiboot_info
    push eax                                                ; multiboot_magic

    extern kernel_main                                      ; External reference to kernel_main
    call kernel_main                                        ; Call kernel_main in main.c

    cli                                                     ; Disable interrupts
.hang:
    hlt                                                     ; Halt the next interrupt
    jmp .hang
.end:
