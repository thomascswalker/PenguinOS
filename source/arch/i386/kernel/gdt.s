section .text
global loadGDT
loadGDT:
    mov     eax,    DWORD [esp + 4]
    lgdt    [eax]
    jmp     0x08:flush
flush:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret
