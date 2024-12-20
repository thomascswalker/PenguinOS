extern gp                                                   ; Declares 'gp' is defined in C.

section .text
global gdt_flush
gdt_flush:
    lgdt [gp]
    ret

global gdt_reload
gdt_reload:
    push 0x08
    lea eax, [rel reload_cs]
    push eax                                                ; Crashes here for some reason
    ret

reload_cs:
    ; Reload segment registers
    mov ax, 0x10                                            ; Load 0x10 into 'ax'. 0x10 is the selector for the data segment in the GDT.
    mov ds, ax                                              ; Load the data segment register
    mov es, ax                                              ; Load the extra segment register
    mov fs, ax                                              ; Load the extra segment register
    mov gs, ax                                              ; Load the extra segment register
    mov ss, ax                                              ; Load the stack segment register
    ret
