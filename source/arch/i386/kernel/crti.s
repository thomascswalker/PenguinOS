; x86 crti.s
section .init_array
global main:function
_init_array:
	push ebp
	mov  ebp, esp											; gcc will nicely put the contents of crtbegin.o's .init section here. */

section .fini_array
global main:function
_fini_array:
	push ebp
	mov  ebp, esp 											; gcc will nicely put the contents of crtbegin.o's .fini section here. */