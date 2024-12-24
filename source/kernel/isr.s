global isr0
global isr1

; Divide by 0
isr0:
    cli                         ; Disable interrupts
    push byte 0                 ;
    push byte 0                 ;
    jmp isr_common_stub         ;

extern fault_handler

isr_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10            ; Load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp            ; Push up the stack

    push eax
    mov eax, fault_handler

    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa

    add esp, 8
    iret