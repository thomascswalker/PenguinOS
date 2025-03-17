section .text
global switchContext
switchContext:
    ; Move the pointers to prev and next from the stack into
    ; registers edi and esi, respectively.
    mov edi, [esp+4]       ; EDI ← pointer to CPUState "prev"
    mov esi, [esp+8]       ; ESI ← pointer to CPUState "next"

    ; Store the current CPUState (into prev)
    pop dword [edi + 24]  ; Pop EBP into prev->ebp
    pop dword [edi + 28]  ; Pop original ESP into prev->esp

    ; Load the next CPUState
    mov ebp, [esi + 24]   ; Next->ebp
    mov esp, [esi + 28]   ; Next->esp

    ; Jump to the instruction pointer of next
    push dword [esi + 56]
    ret
