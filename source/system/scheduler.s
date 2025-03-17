section .text
global switchContext
switchContext:
    ; Move the pointers to prev and next from the stack into
    ; registers edi and esi, respectively.
    mov edi, [esp + 4]          ; EDI ← pointer to CPUState "prev"
    mov esi, [esp + 8]          ; ESI ← pointer to CPUState "next"

    ; Save the current EBP and ESP into the prev CPUState.
    mov [edi + 24], ebp         ; prev->ebp <- EBP
    mov [edi + 28], esp         ; prev->esp <- ESP

    ; Load the next EBP and ESP from the next CPUState.
    mov ebp, [esi + 24]         ; EBP <- next->ebp
    mov esp, [esi + 28]         ; ESP <- next->esp

    ; Restore the flags from the next CPUState.
    push dword [esi + 64]       ; Push flags onto the stack
    popfd                       ; Pop flags into EFLAGS

    push dword [esi + 56]       ; Push next->eip onto the stack
    sti                         ; Enable interrupts
    ret                         ; Return to the next instruction
