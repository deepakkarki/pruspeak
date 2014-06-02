all: pru_firmware

pru_firmware: pru_firmware.obj
	lnkpru pru_firmware.obj AM3359_PRU.cmd -o=pru_firmware

pru_firmware.obj: pru_firmware.c	
	clpru --c99 pru_firmware.c 


