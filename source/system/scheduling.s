section .text
global switchContext
switchContext:
    ; Move the pointers to prev and next from the stack into
    ; registers edi and esi, respectively.
    mov edi, [esp+4]       ; EDI ← pointer to CPUState "prev"
    mov esi, [esp+8]       ; ESI ← pointer to CPUState "next"
    
    ; -------------------------------------- ;
    ; Store the current CPUState (into prev) ;
    ; -------------------------------------- ;

    ; Save segment registers into prev CPUState:
    mov [edi + 0], gs      ; Save gs (offset 0)
    mov [edi + 4], fs      ; Save fs (offset 4)
    mov [edi + 8], es      ; Save es (offset 8)
    mov [edi + 12], ds     ; Save ds (offset 12)
    
    ; Save general-purpose registers using pusha.
    pushfd               ; Save EFLAGS on stack
    pusha                ; Save general-purpose registers
    
    ; At this point, the stack looks like (top-to-bottom):
    ;   EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX, then EFLAGS.

    pop dword [edi + 24]  ; Pop EBP into prev->ebp
    pop dword [edi + 28]  ; Pop original ESP into prev->esp
    pop dword [edi + 32]  ; Pop EBX into prev->ebx
    pop dword [edi + 36]  ; Pop EDX into prev->edx
    pop dword [edi + 40]  ; Pop ECX into prev->ecx
    pop dword [edi + 44]  ; Pop EAX into prev->eax
    pop dword [edi + 64]  ; Pop EFLAGS into prev->eFlags

    ; ---------------------- ;
    ; Load the next CPUState ;
    ; ---------------------- ;

    ; Restore segment registers from next CPUState:
    mov gs, [esi + 0]     ; Load gs from next->gs
    mov fs, [esi + 4]     ; Load fs from next->fs
    mov es, [esi + 8]     ; Load es from next->es
    mov ds, [esi + 12]    ; Load ds from next->ds
    
    ; Restore general-purpose registers in the reverse stack order.
    mov ebp, [esi + 24]   ; Next->ebp
    mov esp, [esi + 28]   ; Next->esp
    mov ebx, [esi + 32]   ; Next->ebx
    mov edx, [esi + 36]   ; Next->edx
    mov ecx, [esi + 40]   ; Next->ecx
    mov eax, [esi + 44]   ; Next->eax

    ; --------------------------------------- ;
    ; Jump to the instruction pointer of next ;
    ; --------------------------------------- ;
    jmp dword [esi + 56] ; Jump to next->eip