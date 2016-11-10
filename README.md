PRU Speak
========

##What is it

PRU Speak is an implementation of the BotSpeak language for the PRU.
This project enables dynamic control of the Beaglebone black's PRU (Programmable Realtime Unit) from the Linux userspace on ARM. For more information have a look at the [official PRU page](http://beagleboard.org/pru) on beagleboard.org

**BotSpeak** is a robotics programming language designed to bridge the gap between small embedded systems such as the Raspberry Pi, BeagleBone, and Arduino.
[See  http://botspeak.org/ and https://sites.google.com/site/botspeak/ for more information]

**PRU** is a dual core micro-controller system present on the AM335x SoC (TI's ARM A8 based chip) which powers the [Beagle Bone black] [1].

The [PRU] [2] is meant to be used for high speed jitter free IO control. Being independent from the linux scheduler and having direct access to the IO pins of the BeagleBone Black, PRU is ideal for offloading IO intensive tasks.

**NOTE** :  PRU Speak will be refactored to use the new remote proc driver for ARM <-> PRU communication, the PRU Bridge. 

##How to use

```
Installing
==========

Install ply (python lex and yacc), make sure pip is installed
#pip install ply

run (to copy firmwares to reqd folder)
#bash install.sh

Environment setup
=================

To load the kernel module, device tree overlay, firmware and TCP server
#bash run.sh

Running
=======

Three possible interfaces :

1. PS shell -
	This is a simple terminal interface (written in python) where you can type commands and scripts and interact with the PRU.
	All the standard command's return values are recieved and printed. 

2. Bash interface -
	This is a bash interface. PRU Speak shows up as a file entry at "/dev/pruspeak".
	All commands can be echoed to the file, eg : root@beaglebone:/dev# echo SET DIO[0], 1 > pruspeak

3. TCP interface -
	A TCP interface at localhost:6060 for the Pru Speak interpreter. Write to the socket for command/script, read for return value.

To try a simple blink example (blink P9_31):
#bash blink.sh

[**NOTE** use only any one interface at a given point of time. PRU Speak is refreshed everytime a interface is initialized. You may lose existing session]

```

##How it works

The project consists of three main components
  * The user space libraries 
  * The remote proc device driver
  * The PRU firmware (PRU implementation of the BotSpeak interpreter)

The system architecture of the project is well illustrated in this flow chart.

![The PRU Speak Architecture](https://goo.gl/J2uwTx)

**The basic workflow is as follows**

1. The user space component consists mainly of python code, and is responsible for interacting with the user. The user has 3 main ways to use PRUSpeak - through an open TCP port, through a FIFO or through an interactive shell. If you're a developer, you can use the direct `pruspeak` python api. 
See examples [here](https://github.com/deepakkarki/pruspeak/blob/master/src/userspace_lib/example_code.py)

2. Create the botspeak script you want to execute, and send it through any of the interfaces. The client side logic will process the commands, compile it to a custom byte code and dump it in a shared memory location. (shared between the userspace process and the PRU0)

3. Once the bytecode is placed, the client side code will signal the same to the PRU0 via sysfs interface. The sysfs interfaces are created by the pruspeak kernel driver, which accepts the system calls made to it by the user space libs and forwards it to the PRUs.

4. he PRU0 has a small VM running on it which can read the bytecode. Once PRU0 gets the control instructions, it will read the bytecode from the shared memory and begin execution.

A simple but more detailed explanation can be found [here](https://goo.gl/09xJfa)

A more detailed description of the design and implementation can be found [here](https://goo.gl/vonpEB)

See some example code [here](https://github.com/deepakkarki/pruspeak/blob/master/src/userspace_lib/test_funcs.py) and [here](https://github.com/deepakkarki/pruspeak/blob/master/src/userspace_lib/example_code.py)

##Build from source

Run ```bash install.sh```

##future work

- [ ] Language bindings to Python and Node.js
- [ ] Cleaner interfaces (for TCP and FIFO --to both r/w)
- [ ] Fill in wiki and encoding scheme
- [ ] Explain working in detail
- [ ] Add streams between PRU and ARM
- [ ] Add ability to dynamically set frequency for PWM
- [ ] More commands for qep, uart, hard-pwm etc
- [ ] Integrate PRU bridge for transportation layer


[1]: http://beagleboard.org/black
[2]: http://elinux.org/BeagleBone_PRU_Notes
