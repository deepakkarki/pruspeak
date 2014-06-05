all: pru_firmware_3

pru_firmware_3: pru_firmware_3.obj pru_firmware_3_asm.obj
	lnkpru pru_firmware_3.obj pru_firmware_3_asm.obj AM3359_PRU.cmd -o=pru_firmware_3

pru_firmware_3.obj: pru_firmware_3.c
	clpru --c99 pru_firmware_3.c 

pru_firmware_3_asm.obj: pru_firmware_3_asm.asm
	clpru pru_firmware_3_asm.asm
