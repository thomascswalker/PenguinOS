extern g_currentProcess
global switchProcess

struc PCB
     .PID                resd 1 ; Offset: 0
     .STATE              resd 1 ; Offset: 4
     .PROGRAM_COUNTER    resd 1 ; Offset: 8
     .STACK_BASE         resd 1 ; Offset: 12
     .STACK_POINTER      resd 1 ; Offset: 16
     .FLAGS              resd 1 ; Offset: 20
     .NEXT               resd 1 ; Offset: 24
endstruc

section .text


; void switchProcess(Process* next)
switchProcess:
     ; __________Stack___________
     ; |------------------------|
     ; |  Return Address (EIP)  | +0
     ; |------------------------|
     ; |       Process* next    | +4
     ; |------------------------|

     ; Push volatile registers (per the 32-bit System V ABI)
     push ebx
     push esi
     push edi
     push ebp

     ; __________Stack___________
     ; |------------------------|
     ; |          EBP           | +0
     ; |------------------------| 
     ; |          EDI           | +4
     ; |------------------------| 
     ; |          ESI           | +8
     ; |------------------------| 
     ; |          EBX           | +12
     ; |-------- ---------------| 
     ; |   Return Address (EIP) | +16
     ; |------------------------| 
     ; |       Process* next    | +20
     ; |------------------------| 

     ; Save g_currentProcess process' state
     mov edi, [g_currentProcess]
     ; EDI now contains the pointer to [Process* g_currentProcess]
     ; |-----|
     ; | EDI | <== [Process* g_currentProcess]
     ; |-----|

     ; Set [g_currentProcess->PCB.STACK_POINTER] to the value in ESP
     ; Process* g_currentProcess->PCB.STACK_POINTER <== ESP
     mov [edi + PCB.STACK_POINTER], esp


     ; Load next process' state
     ; ESP has been decremented by 16, so instead of +4,
     ; we need to do +20. 4 pushes, 1 return address, 4
     ; bytes each.
     mov esi, [esp + 20]
     ; ESI now contains the pointer to [Process* next]
     ;             __________Stack___________
     ;             |------------------------| 
     ;             |   Return Address (EIP) | +16
     ; |-----|     |------------------------|
     ; | ESI | <== |       Process* next    | +20
     ; |-----|     |------------------------|


     ; Set [EXTERN Process* g_currentProcess] to the value in ESI
     ; Process* g_currentProcess <== ESI (Process* next)
     mov [g_currentProcess], esi

     mov esp, [esi + PCB.STACK_POINTER]
     ; |-----|     |-----|
     ; | ESP | <== | ESI |
     ; |-----|     |-----|

     ; Restore non-volatile registers (per the 32-bit System V ABI)
     pop ebp
     pop edi
     pop esi
     pop ebx

     ; The stack is now restored to its original state.
     ; __________Stack___________
     ; |------------------------|
     ; |  Return Address (EIP)  | +0
     ; |------------------------|
     ; |       Process* next    | +4
     ; |------------------------|

     ret