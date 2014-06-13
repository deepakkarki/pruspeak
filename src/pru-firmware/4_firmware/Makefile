all: pru_firmware_4

pru_firmware_4: pru_firmware_4.obj pru_firmware_4_asm.obj
	lnkpru pru_firmware_4.obj pru_firmware_4_asm.obj AM3359_PRU.cmd -o=pru_firmware_4

pru_firmware_4.obj: pru_firmware_4.c pru_firmware.h
	clpru --c99 pru_firmware_4.c 

pru_firmware_4_asm.obj: pru_firmware_4_asm.asm
	clpru pru_firmware_4_asm.asm
