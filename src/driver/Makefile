obj-m += pru_speak.o
KDIR := /lib/modules/$(shell uname -r)/build

all:
	if [ ! -d "$(KDIR)" ]; then apt-get install linux-headers-$(shell uname -r); fi
	make -C $(KDIR) M=$(PWD) modules

load: pru_speak.ko unload
	insmod ./pru_speak.ko

unload:
	rmmod ./pru_speak.ko || true

install:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules_install

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

