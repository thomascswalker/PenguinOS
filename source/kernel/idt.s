section .text

global load_idt
load_idt:
    mov eax, [esp + 4]                                      ; Get first param, pointer to our IDT
    lidt [eax]                                              ; Load the IDT pointer
    ret

%macro ISR_NOERRORCODE 1
global isr%1
isr%1:
    cli
    push 0
    push %1                                                 ; Push interrupt number
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    push %1                                                 ; Push error code
    jmp isr_common_stub
%endmacro

ISR_NOERRORCODE 0
ISR_NOERRORCODE 1
ISR_NOERRORCODE 2
ISR_NOERRORCODE 3
ISR_NOERRORCODE 4
ISR_NOERRORCODE 5
ISR_NOERRORCODE 6
ISR_NOERRORCODE 7
ISR_ERRCODE 8
ISR_NOERRORCODE 9
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14
ISR_NOERRORCODE 15
ISR_NOERRORCODE 16
ISR_NOERRORCODE 17
ISR_NOERRORCODE 18
ISR_NOERRORCODE 19
ISR_NOERRORCODE 20
ISR_NOERRORCODE 21
ISR_NOERRORCODE 22
ISR_NOERRORCODE 23
ISR_NOERRORCODE 24
ISR_NOERRORCODE 25
ISR_NOERRORCODE 26
ISR_NOERRORCODE 27
ISR_NOERRORCODE 28
ISR_NOERRORCODE 29
ISR_NOERRORCODE 30
ISR_NOERRORCODE 31

extern isr_handler
isr_common_stub:
    pusha                                                   ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    mov ax, ds                                              ; Lower 16-bits of eax = ds.
    push eax                                                ; save the data segment descriptor

    mov ax, 0x10                                            ; load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handler

    pop ebx                                                 ; reload the original data segment descriptor
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa                                                    ; Pops edi,esi,ebp...
    add esp, $8                                             ; Cleans up the pushed error code and pushed ISR number
    sti
    iret   
                                                            ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
%macro IRQ 2
global irq%1
irq%1:
    cli
    push %2                                                 ; Push interrupt number
    push 0                                                  ; Push default error code of 0
    jmp irq_common_stub
%endmacro

; global irq0
; irq0:
;     cli
;     push 0
;     push 32
;     pusha
;     call irq_handler
;     popa
;     add esp, 8
;     sti
;     iret
    
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

extern irq_handler
irq_common_stub:
    pusha

    mov	    ax, ds
    push	eax
    mov 	esi, eax

    mov 	ax, 10h
    mov 	ds, ax
    mov 	es, ax
    mov 	fs, ax
    mov 	gs, ax

    call	irq_handler

    pop	    ebx
    mov 	ebx, esi

    mov 	ds, bx
    mov 	es, bx
    mov 	fs, bx
    mov 	gs, bx

    popa
    add     esp, 8
    sti
    iret