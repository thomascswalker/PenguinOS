section .text

global loadIDT
loadIDT:
    mov eax, [esp + 4]                                      ; Get first param, pointer to our IDT
    lidt [eax]                                              ; Load the IDT pointer
    ret

%macro ISR_NOERRORCODE 1
global isr%1
isr%1:
    cli
    push 0
    push %1                                                 ; Push interrupt number
    jmp isrCommon
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    push %1                                                 ; Push error code
    jmp isrCommon
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

ISR_NOERRORCODE 128

extern isrHandler
isrCommon:
    pusha                    ; Push all general-purpose registers onto the stack

    ; Push all segment registers onto the stack
    mov     ax, gs           ; Move the value of the gs segment register into ax
    push    eax              ; Push the value of ax (which is the value of gs) onto the stack
    mov     ax, fs           ; Move the value of the fs segment register into ax
    push    eax              ; Push the value of ax (which is the value of fs) onto the stack
    mov     ax, es           ; Move the value of the es segment register into ax
    push    eax              ; Push the value of ax (which is the value of es) onto the stack
    mov     ax, ds           ; Move the value of the ds segment register into ax
    push    eax              ; Push the value of ax (which is the value of ds) onto the stack
    mov     esi, eax         ; Move the value of ax into esi (save the original ds value)

    mov     ax, 0x10         ; Move the value 0x10 (kernel data segment selector) into ax
    mov     ds, ax           ; Load the kernel data segment selector into ds
    mov     es, ax           ; Load the kernel data segment selector into es
    mov     fs, ax           ; Load the kernel data segment selector into fs
    mov     gs, ax           ; Load the kernel data segment selector into gs

    call    isrHandler       ; Call the C function isrHandler to handle the interrupt

    pop     ebx              ; Pop the top value from the stack into ebx (original ds value)
    mov     ebx, esi         ; Move the saved original ds value from esi into ebx

    mov     ds, bx           ; Restore the original value of ds from bx
    mov     es, bx           ; Restore the original value of es from bx
    mov     fs, bx           ; Restore the original value of fs from bx
    mov     gs, bx           ; Restore the original value of gs from bx

    popa                    ; Pop all general-purpose registers from the stack
    add     esp, 8           ; Adjust the stack pointer to remove the error code and interrupt number
    sti                      ; Set the interrupt flag (enable interrupts)
    iret                     ; Return from the interrupt handler

%macro IRQ 2
global irq%1
irq%1:
    cli
    push 0                                                  ; Push default error code of 0
    push %2                                                 ; Push interrupt number
    jmp irqCommon
%endmacro

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

extern irqHandler
irqCommon:
    pusha                    ; Push all general-purpose registers onto the stack

    mov     ax, gs           ; Move the value of the gs segment register into ax
    push    eax              ; Push the value of ax (which is the value of gs) onto the stack
    mov     ax, fs           ; Move the value of the fs segment register into ax
    push    eax              ; Push the value of ax (which is the value of fs) onto the stack
    mov     ax, es           ; Move the value of the es segment register into ax
    push    eax              ; Push the value of ax (which is the value of es) onto the stack
    mov     ax, ds           ; Move the value of the ds segment register into ax
    push    eax              ; Push the value of ax (which is the value of ds) onto the stack
    mov     esi, eax         ; Move the value of ax into esi (save the original ds value)

    mov     ax, 0x10         ; Move the value 0x10 (kernel data segment selector) into ax
    mov     ds, ax           ; Load the kernel data segment selector into ds
    mov     es, ax           ; Load the kernel data segment selector into es
    mov     fs, ax           ; Load the kernel data segment selector into fs
    mov     gs, ax           ; Load the kernel data segment selector into gs

    call    irqHandler       ; Call the C function irqHandler to handle the interrupt

    pop     ebx              ; Pop the top value from the stack into ebx (original ds value)
    mov     ebx, esi         ; Move the saved original ds value from esi into ebx

    mov     ds, bx           ; Restore the original value of ds from bx
    pop     eax              ; Pop the original value of es from the stack into eax
    mov     es, ax           ; Restore the original value of es from ax
    pop     eax              ; Pop the original value of fs from the stack into eax
    mov     fs, ax           ; Restore the original value of fs from ax
    pop     eax              ; Pop the original value of gs from the stack into eax
    mov     gs, ax           ; Restore the original value of gs from ax

    popa                    ; Pop all general-purpose registers from the stack
    add     esp, 8           ; Adjust the stack pointer to remove the error code and interrupt number
    sti                      ; Set the interrupt flag (enable interrupts)
    iret                     ; Return from the interrupt handler