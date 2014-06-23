obj-m += pru_speak.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

load: pru_speak.ko unload
	insmod ./pru_speak.ko

unload:
	rmmod ./pru_speak.ko || true

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

