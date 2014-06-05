all: pru_firmware_2

pru_firmware_2: pru_firmware_2.obj pru_firmware_2_asm.obj
	lnkpru pru_firmware_2.obj pru_firmware_2_asm.obj AM3359_PRU.cmd -o=pru_firmware_2

pru_firmware_2.obj: pru_firmware_2.c
	clpru --c99 pru_firmware_2.c 

pru_firmware_2_asm.obj: pru_firmware_2_asm.asm
	clpru pru_firmware_2_asm.asm
