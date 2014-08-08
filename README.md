PRU Speak
========

##What is it

PRU Speak is an implementation of the BotSpeak language for the PRU.
This project enables dynamic control of the Beaglebone black's PRU (Programmable Realtime Unit) from the Linux userspace on ARM.

**BotSpeak** is a robotics programming language designed to bridge the gap between small embedded systems such as the Raspberry Pi, BeagleBone, and Arduino.
[See  http://botspeak.org/ and https://sites.google.com/site/botspeak/ for more information]

**PRU** is a dual core micro-controller system present on the AM335x SoC (TI's ARM A8 based chip) which powers the [Beagle Bone black][http://beagleboard.org/black].

The [PRU][http://elinux.org/BeagleBone_PRU_Notes] is meant to be used for high speed jitter free IO control. Being independent from the linux scheduler and having direct access to the IO pins of the BeagleBone Black, PRU is ideal for offloading IO intensive tasks.

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

To load the kernel module and device tree overlay
#bash setup.sh

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
![The PRU Speak Architecture](https://raw.githubusercontent.com/wiki/deepakkarki/pruspeak/PRU-Speak-Architecture.png)

##Project structure
-- TODO : explain various dirs and files. 

##future work
-- TODO:
	Language bindings

	cleaner interfaces (for TCP and FIFO --to both r/w)

	fill in wiki and encoding scheme

	explain working in detail

##more detail on how to use
-- TODO
	language feature and limitations and 'gotcha' points ;)

	[link to some other page (wiki maybe)]

##examples
-- TODO
	Lots of examples including 

	[link to some other page (wiki maybe)]

