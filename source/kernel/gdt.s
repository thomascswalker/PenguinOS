[bits 32]

section .text
global gdt_flush
gdt_flush:
    mov eax, esp
    lgdt [eax]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ss, ax
    mov gs, ax

    ; Jump to .flush at 0x08
    jmp 0x08:.flush     ; segment:offset
.flush:
    ; Return to gdt.h
    ret