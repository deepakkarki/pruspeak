#PRU Speak Driver
=================

This is the remote proc driver for the PRU Speak firmware. 

This driver is responsible for :
> Loading the PRU Speak firmware on the PRU.
> Exposing a sysfs interface for exchanging BotSpeak instructions.
> Providing mmap functionality for fast IO between the client process and PRU.
> Forwarding interrupts to the userspace.
