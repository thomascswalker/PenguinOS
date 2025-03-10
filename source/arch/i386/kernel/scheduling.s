; context_switch.asm - NASM-style assembly for context switching (x86 32-bit)
; Function prototype: void context_switch(uint32_t **old_sp, uint32_t *new_sp)
;
; Calling convention (cdecl):
;   [esp + 4]  : pointer to the location where the current SP should be stored (old_sp)
;   [esp + 8]  : new stack pointer to load (new_sp)
;
; After pushing 4 registers (ebp, ebx, esi, edi), the offsets to the arguments become:
;   old_sp is at [esp + 20] and new_sp is at [esp + 24].

section .text

global switchTo
switchTo:
    push ebp
    push ebx
    push esi
    push edi

    ; After pushing 4 registers, the stack layout is:
    ; [esp+0] : return address (for __switch_to's caller)
    ; [esp+4] : saved edi
    ; [esp+8] : saved esi
    ; [esp+12]: saved ebx
    ; [esp+16]: saved ebp
    ; [esp+20]: first argument (prev pointer)
    ; [esp+24]: second argument (next pointer)
    ;

    ; Now ESP = original ESP - 16.
    ; Retrieve the arguments:
    mov eax, [esp + 20]  ; eax = pointer to old_sp
    mov [eax + 4], esp

    mov edx, [esp + 24]  ; edx = new_sp
    mov esp, [edx + 4]

    ; Restore the callee-saved registers from the new context.
    pop edi
    pop esi
    pop ebx
    pop ebp
    
    ret