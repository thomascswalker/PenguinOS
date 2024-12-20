; x86 crti.s
section .init
global main:function
_init:
	push ebp
	mov  ebp, esp											; gcc will nicely put the contents of crtbegin.o's .init section here. */

section .fini
global main:function
_fini:
	push ebp
	mov  ebp, esp 											; gcc will nicely put the contents of crtbegin.o's .fini section here. */