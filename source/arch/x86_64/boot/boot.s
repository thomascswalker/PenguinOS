; boot.s – A basic multiboot bootloader that sets up 64‑bit long mode

;-------------------------------------------------------------------------
; 32-bit initial code (multiboot header MUST be in 32-bit mode)
;-------------------------------------------------------------------------
[bits 32]

; Multiboot header definitions
MB_ALIGN        equ 1 << 0
MB_MEMORY_INFO  equ 1 << 1
MB_GFX          equ 0
MB_FLAGS        equ MB_ALIGN | MB_MEMORY_INFO | MB_GFX
MB_MAGIC        equ 0x1BADB002
MB_CHECKSUM     equ -(MB_MAGIC + MB_FLAGS)

section .multiboot
align 4
    dd MB_MAGIC
    dd MB_FLAGS
    dd MB_CHECKSUM
    dd 0, 0, 0, 0, 0

; Allocate a 16KB stack for 32-bit mode
section .bss
align 16
stack32:
    resb 16384

;-------------------------------------------------------------------------
; 32-bit code: _start
;-------------------------------------------------------------------------
section .text
global _start
extern kmain   ; kmain will be provided by C

_start:
    cli                             ; Disable interrupts
    mov esp, stack32 + 16384        ; Set up the 32-bit stack

    ;-----------------------------------------------------------
    ; Set up the Global Descriptor Table (GDT)
    ;-----------------------------------------------------------
    call setup_gdt_64             ; Load our new GDT

    ;-----------------------------------------------------------
    ; Set up basic paging (identity map first 2MB using 2MB page)
    ;-----------------------------------------------------------
    call setup_paging             ; Build the PML4, PDPT, and PD

    ; Load CR3 with the physical address of our PML4 table.
    mov eax, pml4
    mov cr3, eax

    ; Enable PAE in CR4 (set bit 5)
    mov eax, cr4
    or eax, 0x20                  ; 0x20 = (1 << 5)
    mov cr4, eax

    ;-----------------------------------------------------------
    ; Enable Long Mode:
    ; Set LME (Long Mode Enable) in IA32_EFER (MSR 0xC0000080)
    ;-----------------------------------------------------------
    mov ecx, 0xC0000080
    rdmsr                         ; EDX:EAX <- IA32_EFER
    or eax, 0x100                 ; Set bit 8: LME
    xor edx, edx                  ; Clear upper 32 bits
    wrmsr

    ; Enable paging by setting the PG bit in CR0 (bit 31)
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    ;-----------------------------------------------------------
    ; Now perform a far jump into 64-bit mode.
    ; Selector 0x08 is our 64-bit code segment (see GDT below)
    ;-----------------------------------------------------------
    jmp 0x08:long_mode_entry

;-------------------------------------------------------------------------
; GDT Setup (must be in low memory; used by both modes)
;-------------------------------------------------------------------------
section .gdt
align 8
gdt_start:
    dq 0x0000000000000000         ; Null descriptor
    ; 64-bit code descriptor:
    ; 0x00AF9A000000FFFF is a common value for a long mode code segment:
    ;  - 0x9A: type=code, executable, readable, accessed; 0xA = 1010 (L bit set)
    ;  - Base and limit are ignored in long mode.
    dq 0x00AF9A000000FFFF
    ; Data segment descriptor:
    dq 0x00CF92000000FFFF         ; Read/Write data segment
gdt_end:

gdt_ptr:
    dw gdt_end - gdt_start - 1    ; Limit
    dq gdt_start                  ; Base address

; Routine to load the new GDT.
setup_gdt_64:
    lgdt [gdt_ptr]
    ret

;-------------------------------------------------------------------------
; Paging Tables – defined in .data
; We allocate each table aligned to 4K.
; Each entry is 8 bytes; here we assume all tables are in low memory.
;-------------------------------------------------------------------------
section .data
align 4096
; PML4 table: 512 entries, each 8 bytes. We'll fill only entry 0.
pml4:
    times 512 dd 0, 0             ; Each entry: lower dword then upper dword

align 4096
; PDPT table: 512 entries; only entry 0 is used.
pdpt:
    times 512 dd 0, 0

align 4096
; Page Directory (PD): 512 entries; we use a 2MB page so we only fill entry 0.
pd:
    times 512 dd 0, 0

; Setup paging routine (in 32-bit mode, using 32-bit registers).
; We write each 64-bit entry as two consecutive 32-bit words.
setup_paging:
    ; -- Setup PML4[0]: point to PDPT table with flags (present + rw) --
    mov eax, pdpt               ; Base address of PDPT (assumed < 4GB)
    or eax, 0x03                ; Flags: Present (0x1) and Read/Write (0x2)
    mov dword [pml4], eax
    mov dword [pml4+4], 0

    ; -- Setup PDPT[0]: point to PD table --
    mov eax, pd                ; Base address of PD
    or eax, 0x03             ; Flags: Present + Read/Write
    mov dword [pdpt], eax
    mov dword [pdpt+4], 0

    ; -- Setup PD[0]: Map first 2MB of memory as a 2MB page --
    ; The PD entry for a 2MB page has the PS (Page Size) bit (bit 7) set.
    mov eax, 0x00000000         ; Physical base address (identity map)
    or eax, 0x83                ; Flags: Present (0x1) | Read/Write (0x2) | PS (0x80)
    mov dword [pd], eax
    mov dword [pd+4], 0
    ret

;-------------------------------------------------------------------------
; 64-bit Code Section
; Switch assembler to 64-bit mode
;-------------------------------------------------------------------------
[bits 64]
long_mode_entry:
    ; Allocate a 16KB stack for 64-bit mode.
    ; The 64-bit stack is defined in the .bss section below.
    mov rsp, stack64 + 16384

    ; Now call the C kernel entry point.
    extern kmain
    call kmain

hang:
    cli
    hlt
    jmp hang

;-------------------------------------------------------------------------
; 64-bit Stack
;-------------------------------------------------------------------------
section .bss
align 16
stack64:
    resb 16384