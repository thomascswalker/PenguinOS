; x86 crtn.s */
section .init_array
	; gcc will nicely put the contents of crtend.o's .init section here. */
	pop ebp
	ret

section .fini_array
	; gcc will nicely put the contents of crtend.o's .fini section here. */
	pop ebp
	ret