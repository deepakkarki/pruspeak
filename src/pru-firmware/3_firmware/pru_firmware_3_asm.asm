	.text
	.global sc_downcall
sc_downcall:
	MOV R0.w0, R14.w0		;* save the pointer to the handler function
	;* first issue the downcall ready
	LDI R14, 254			;* DOWNCALL_READY
	LDI R31, 35			;* SYSCALL_VALUE
	HALT				;* host must save R3.w0 locally
	;* the host will manipulate our state so that the arguments are correct
	JAL R3.w2, R0.w0		;* call
	MOV R0, R14			;* save the return code
	;* when we return here, we will inform the host of the result
	LDI R14, 255			;* DOWNCALL_DONE
	LDI R31, 35			;* SYSCALL_VALUE
	HALT				;* host must return to save R3.w0
