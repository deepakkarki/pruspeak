PRU Speak firmware
===================

This is the firmware that gets loaded on the PRU by the driver on loading the Device Tree Overlay (DTO).

This firmware behaves as a BotSpeak bytecode interpreter. Handles commands and instruction/data streams from the ARM CPU.

Build
=====
* Download ARM installer for PRU compiler on BeagleBone from http://software-dl.ti.com/codegen/non-esd/downloads/beta.htm
* Make installer executable and run it from your desired install directory
* Set environment variables (example placed in .bashrc)
  export MANPATH=$MANPATH:$HOME/pru_2.0.0B2/man
  export PATH=$PATH:$HOME/pru_2.0.0B2/bin
  export PRU_C_DIR=$HOME/pru_2.0.0B2/lib
  export PRU_A_DIR=$HOME/pru_2.0.0B2/lib
* make
