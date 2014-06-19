all: pru0_firmware

pru0_firmware: pru0_firmware.obj pru0_syscall.obj
	lnkpru pru0_firmware.obj pru0_syscall.obj AM3359_PRU.cmd -o=pru0_firmware

pru0_firmware.obj: pru0_firmware.c pru_firmware.h
	clpru --c99 pru0_firmware.c 

pru0_syscall.obj: pru0_syscall.asm
	clpru pru0_syscall.asm
