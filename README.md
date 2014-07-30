PRU Speak
========

PRU Speak is a project which enables dynamic control of the Beaglebone black's PRU (Programmable Realtime Unit) from the Linux userspace on ARM.

PRU Speak is an implementation of the BotSpeak language for the PRU. 
BotSpeak is a robotics programming language designed to bridge the gap between small embedded systems such as the Raspberry Pi, BeagleBone, and Arduino.

See https://sites.google.com/site/botspeak/ and http://botspeak.org/ for more info.

The project consists of three main components
  * The user space libraries 
  * The remote proc device driver
  * The PRU firmware (PRU implementation of the BotSpeak interpreter)

##Using
```
Installing
==========

Install ply (python lex and yacc) : pip install ply

cd bin
insmod pru_speak.ko
echo BB-BONE-PRU-07 > /sys/devices/bone_capemgr.9/slots

Running
=======
cd ../src/userspace_lib/
python bs_shell.py
[The PRU Speak shell is now opened :)] 
```

Botspeak Gammar in Extended Backus-Naur Form (EBNF) (http://en.wikipedia.org/wiki/Extended_Backus-Naur_Form)

```
<comment>  ::= from # to end-of-line
<reserved> ::= IF | GOTO | WAIT | ADD | SUB | DIV | MUL 
  | SETVAR | GETVAR | LBL | SCRIPT | ENDSCRIPT
  | RUN | ABORT | SYSTEM | DEBUG
<ident>  ::= letter, {letter | digit} except <reserved>
<k>      ::= <decimal-literal>
<compop> ::= < | > | <= | >= | != | ==
<array>  ::= <ident>"["(<k> | <ident>)"]"
<value>  ::= <k> | <ident> | <array>
<instr>  ::= IF "("<value> <compop> <value>")" <instr> except IF
  | GOTO <value>
  | WAIT <value>
  | ADD <value>","<value>
  | SUB <value>","<value>
  | DIV <value>","<value>
  | MUL <value>","<value>
  | SETVAR <value>","<value>
  | GETVAR <value>","<value>
  | LBL <ident>
  | RUN [SCRIPT"["<value>"]"]
  | ABORT [SCRIPT"["<value>"]"]
  | DEBUG
<line>    ::= (<instr> | <comment>),end-of-line
<program> ::= SCRIPT {<line> | (<comment>} ENDSCRIPT,end-of-line

TBD: SYSTEM, RUN&WAIT
```

The system architecture of the project is well illustrated in this flow chart.
![The PRU Speak Architecture](https://raw.githubusercontent.com/wiki/deepakkarki/pruspeak/PRU-Speak-Architecture.png)


