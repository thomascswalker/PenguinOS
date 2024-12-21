[bits 32]

global gdt_flush
gdt_flush:
    push ebp                ; Save old call frame
    mov ebp, esp            ; Initialize new call frame

    mov eax, [ebp + 8]      ; *(uint32*)(ebp + 8);
    lgdt [eax]              ; Load GDT with 'lgdt'

    mov eax, [ebp + 12]     ; *(uint32*)(ebp + 12);
    push eax

    jmp 0x08:reload_cs
    retf

reload_cs:
    ; reload data segments
    mov ax, [ebp + 16]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; restore old call frame
    mov esp, ebp
    pop ebp

    ret
