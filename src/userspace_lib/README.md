#PRU Speak encoding

> NOTE : The short forms used in this encoding doc are as follows :

>         C : constant (value is the constant specified)

>         V : variable (value is the address where variable V is located)

>         Arr[C] => V (since, the final value is address(Arr) + C, viz a variable). 
  Note that V will be used instead of Arr[C] throughout the document.
  
>         Arr[V] : array indexed by variable V

(I should probably write how the variable stuff work here...
and also how the doc is structured)

##1. SET instruction
The SET instruction is of the form 

```
SET x, y
```
where **x** (operand-1) can be either **V or Arr[v]** whereas **y** (operand-2) can be either **C, V, Arr[v]**.

The general function of this instruction is to set variable values.
(In some cases it is also used to create variables)

There are many possible ways a **SET** instruction can be used based on how we choose **x** and **y**
There are three possible opcodes (as given below) that handle all conditions of x, y.

> OPCODE used : 0x10 (16) to 0x12 (18)

###1.a ```SET V, C```

This is a 32 bit instruction. This instruction is the case when operand-1 is a **V or A[c]** and opernad-2 is a **constant (max 16 bit)**. 

**opcode** 

```c
0x10 (16)
```

**operation**
```
Sets the value of variable V to the 16 bit int value C.
If V has not been previously defined, it creates and assigns.
```

**encoding format** (each value separated by a comma takes one byte)
```c
OPCODE, VARIABLE, CONST-hi, CONST-lo

Byte3 : 0001-0000 : (OPCODE - Fixed)
Byte2 : VARIABLE  : (address of the variable V)
Byte1 : CONST-hi  : (MS-byte of constant C)
Byte0 : CONST-lo  : (LS-byte of constant C)

```

**Example**
```
coming soon :)
```

###1.b ```SET V1, V2```

This is a 32 bit instruction. This instruction is the case when operand-1 and operand-2 are either **V** (or Arr[c]) 

**opcode** 

```c
0x11 (17)
```

**operation**
```
Sets the value of variable V1 to the value of variable V2.
If V1 has not been previously defined, it creates and assigns.
(give some more explanation? or put that in the examples? - I think in example is better)
```

**encoding format** (each value separated by a comma takes one byte)
```c
OPCODE, VARIABLE, unused, VARIABLE

Byte3 : OPCODE    : 0001-0001   (OPCODE - Fixed)
Byte2 : VARIABLE  : (address of the variable V1)
Byte1 : unused    : (unused)
Byte0 : VARIABLE  : (address of the variable V2)

```

**Example**
```
coming soon :)
```

###1.c ```SET x, y```

This is a **64 bit** instruction. This instruction is the case when either **x** (operand-1) or **y** (operand-2) 
is of the form **Arr[v]**.

The possible forms of instructions :
```
SET V1, Arr[V2]
SET Arr[V], 280
SET Arr[V1], V2
SET Arr[V1], Arr[V2]
```

**opcode** 

```c
0x12 (18)
```

**operation**
```
Sets the value of variable "x" to the value of "y"
If "x" is a variable (not Arr[v]) and has not been previously defined, it creates and assigns.
If "x" is of type Arr[v], and Arr has not been defined, it is an error.
```

**encoding format** (each value separated by a comma takes one byte)
```
OPCODE, OP_type , OP1.b, OP1.a
unused, unused  , OP2.b, OP2.a

(OP1 : x, OP2 : y)

Byte3 : OPCODE    : 0001-0010   (OPCODE - Fixed)
Byte2 : OP_type   : (operand type - since the operands can be of any type, this indicates what the operands are)
Byte1 : OP1.b     : (reserved for operator 1, actual value depends on OP_decider)
Byte0 : OP1.a     : (reserved for operator 1, actual value depends on OP_decider)

Byte4 : OP2.b     : (reserved for operator 2, actual value depends on OP_decider)
Byte5 : OP2.a     : (reserved for operator 2, actual value depends on OP_decider)
Byte6 : unused    : unused
Byte7 : unused    : unused


OP_Type (8 bits)

00  00  - 0000
\ / \ /   \  /
 x   y    unused
 
  bit 7,6 : type of operand x
  bit 5,4 : type of operand y

bit encoding that indicate what the corresponding operators are :
(and what they represent in the encoding in each case)

00 => C (Constant)
  OP.a : Const-lo
  OP.b : Const-hi
  
01 => V (Variable)
  OP.a : Variable address of V
  OP.b : unused
  
10 => Arr[V] 
  OP.a : Variable address of V
  OP.b : Array address of Arr.
  
(add a simple example here)
```

**Example**
```
coming soon :)
```


##2. SET RES[] instruction

This instruction is of the form 
```
SET RES[x], y
```
Where **RES** is a reserved array variable such as **DIO, PWM, COM, TMR, AIO**;

**x** can be either **C or V** where as **y** (operand-2) can be either **C, V, Arr[v]**.

This behaves like a regular SET instruction, but has been given special opcodes as :
  * These instruction use reserved arrays
  * They are used often, so giving exclusive byte code makes execution faster on the PRU
  * Normally these sort of instructions will take 64bits; whereas here all take only 32bits

Hence these inst are not compiled as a part of normal **SET** instruction.

> OPCODE used : 0x01 (1) to 0xFF (15)

###2.a ```SET DIO[x], y```

This is a 32 bit instruction. Here **x** and **y** can be either a Constant (C) or Vriable(V).

The possible forms of the instruction :

```
SET DIO[C], C
SET DIO[C], V
SET DIO[V], C
SET DIO[V], V
```

**opcode**
```c
0x01 (01)
```

**operation**
```
Sets the specified digital output to either high or low
(see more in the examples)
```

**encoding format**
```
OPCODE, OP_type, OP1, OP2

Byte3 : OPCODE    : 0000-0001   (OPCODE - Fixed)
Byte2 : OP_type   : (operand type - this indicates what type the operands are)
Byte1 : OP1       : (not exactly operand1, but rather 'x' in DIO[x])
Byte0 : OP2       : (operand2, (i.e.) "y"))

Note : actual value of OP1 and OP2 will depend on type of "x" and "y".
       and the type information is encoded in OP_type as given below.
       
OP_type (making sense of the 8 bits)

0 0 00-0000
| | \     /
x y  unused

  bit 7 - type of "x"
  bit 6 - type of "y"

semantics 
  0 => type is constant C
  1 => type is Variable V
  
(add a simple example here)
```

**Example**
```
coming soon :)
```

###2.b ```SET DIO[C], Arr[V]```

This is a 32 bit instruction. Here first operand is **DIO** indexed by a Constant (C) and the second operand is a variable indexed Array (Arr[v]).

**opcode**
```c
0x02 (02)
```

**operation**
```
Sets the specified digital output to either high or low
(see more in the examples)
```

**encoding format**
```
OPCODE, CONST, Arr, VARIABLE

Byte3 : OPCODE    : 0000-0010   (OPCODE - Fixed)
Byte2 : CONST     : CONSTANT value C
Byte1 : Arr       : Array address
Byte0 : VARIABLE  : address of variable V

(add a simple example here)
```

**Example**
```
coming soon :)
```

###2.c ```SET DIO[V1], Arr[V2]```

This is a 32 bit instruction. Here first operand is **DIO** indexed by a Variable (V1) and the second operand is a variable indexed Array (Arr[v2]).

**opcode**
```c
0x03 (03)
```

**operation**
```
Sets the specified digital output to either high or low
(see more in the examples)
```

**encoding format**
```
OPCODE, VARIABLE, Arr, VARIABLE

Byte3 : OPCODE    : 0000-0010   (OPCODE - Fixed)
Byte2 : VARIABLE  : address of variable V1
Byte1 : Arr       : Array address
Byte0 : VARIABLE  : the array index value - variable value V2

(add a simple example here)
```

**Example**
```
coming soon :)
```


###2.d ```SET PWM[x], y```

This is a 32 bit instruction. Here **x** and **y** can be either a Constant (C) or Vriable(V).

The possible forms of the instruction :

```
SET PWM[C], C
SET PWM[C], V
SET PWM[V], C
SET PWM[V], V
```

**opcode**
```c
0x04 (04)
```

**operation**
```
Sets the specified PWM output to the mentioned value
(see more in the examples)
```

**encoding format**
```
Same as 2.a (change only opcode)
```

**Example**
```
coming soon :) - or see 2.a again?
```

###2.e ```SET PWM[C], Arr[V]```

This is a 32 bit instruction. Here first operand is **DIO** indexed by a Constant (C) and the second operand is a variable indexed Array (Arr[v]).

**opcode**
```c
0x05 (05)
```

**operation**
```
Sets the specified PWM output to the mentioned value
(see more in the examples)
```

**encoding format**
```
same as 2.a
```

**Example**
```
coming soon :)
```

###2.f ```SET PWM[V1], Arr[V2]```

This is a 32 bit instruction. Here first operand is **DIO** indexed by a Variable (V1) and the second operand is a variable indexed Array (Arr[v2]).

**opcode**
```c
0x06 (06)
```

**operation**
```
Sets the specified PWM output to the mentioned value
(see more in the examples)
```

**encoding format**
```
same as 2.a
```

**Example**
```
coming soon :)
```

###2.x AIO, TMR, COM Instructions

> Just like DIO & PWM, the above mentioned instructions have 3 opcodes for each of the following instruction type
  * SET RES[C], C/V
  * SET RES[C], Arr[V]
  * SET RES[V], Arr[V]
  
**Only** things that change are the **operation** and **OPCODE**. (encoding and examples are more or less the same)

####Operation
```
AIO : Sets the value of the anolog output
COM : Writes the value to the indexed com port.
TMR : Sets the timer value
```

####OPCODE
```
AIO : 0x07, 0x08, 0x09
COM : 0x0A, 0x0B, 0x0C
TMR : 0x0D, 0x0E, 0x0F
```

##3. IF instruction

This instruction is of the form 
```
IF ( x <cond> y ) GOTO z
```
Where **x** (first operand), **y** (second operand), **z** (third operand) can be any of C, V, Arr[V].
Here <cond> is a relational operator such as '>', '<', '==', '!=' etc.

There is no different cases for this instruction, rather one **64 bit** instruction which handles all possible variable types internally. 

**opcode**
```python
The **OPCODE** starts from 0x20 (0b0010-0000) and ends at 0x2F (0b0010-1111).
How this works is this instruction reserves the "0b0010-xxxx" instruction space.
The last 4 bits takes its value based upon <cond>. The mapping is as shown below.

cond_code = {
				'=='	: 0b0000,
				'!='	: 0b0001,
				'>='	: 0b0010,
				'<='	: 0b0011,
				'>'	: 0b0100,
				'<'	: 0b0101
	}

```

**operation**
```
The comparision is carried out between "x" and "y", if the result is true, then the IP (inst pointer) jumps to the inst number specified by "x"
```

**encoding**
```
OPCODE, OP_type, OP1.b, OP1.a
OP3.b ,	OP3.a,	 OP2.b, OP2.a

(x : OP1, y: OP2, z : OP3)

Byte3 : OPCODE    : 0001-0010   (OPCODE - Fixed)
Byte2 : OP_type   : (operand type - since the operands can be of any type, this indicates what the operands are)
Byte1 : OP1.b     : (reserved for operator 1, actual value depends on OP_decider)
Byte0 : OP1.a     : (reserved for operator 1, actual value depends on OP_decider)

Byte4 : OP2.b     : (reserved for operator 2, actual value depends on OP_decider)
Byte5 : OP2.a     : (reserved for operator 2, actual value depends on OP_decider)
Byte6 : OP3.a	  : (reserved for operator 3, actual value depends on OP_decider)
Byte7 : OP3.b     : (reserved for operator 3, actual value depends on OP_decider)

Note : actual value of OP1 and OP2 will depend on type of "x" and "y".
       and the type information is encoded in OP_type as given below.
       
OP_type (making sense of the 8 bits)

00  00  - 00  00
\ / \ /  \  /\  /
 x   y    z  unused
 
  bit 7,6 : type of operand x
  bit 5,4 : type of operand y
  bit 3,2 : type of operand z
  
bit encoding that indicate what types the corresponding operators are :
(and how they are placed in the byte encoding in each case)

00 => C (Constant)
  OP.a : Const
  OP.b : unused
  
01 => V (Variable)
  OP.a : Variable address of V
  OP.b : unused
  
10 => Arr[V] 
  OP.a : Variable address of V
  OP.b : Array address of Arr.
  
(add a simple example here)
```

**Example**
```
coming soon :)
```

##4. WAIT instruction
This instruction is of the form 
```
WAIT x
```
where **x** (the only operand) can be of type **C or V or Arr[v]**

This is a 32 bit instruction.

**opcode**
```c
0x14 (20)
```
**operation**
```
Executing this command will pause the execution of the running script for 
"x" mili seconds. (The wait is non blocking - more about this later)
```
**encoding**
```
OPCODE, OP_type, OP.b, OP.a

since the operand can be any of three types, the OP_type will tell us the 
type of "x" in the instruction.

The OP_type is encoded as follows

00  00 - 0000
\/  \       /
x    unused

bit value to type of OP ("x") mapping :
00 => OP is of type 'C'
  OP.a = Const
  OP.b = unused
  
01 => OP is of type 'V'
  OP.a = VARIABLE
  OP.b = unused
  
10 => OP is of type Arr[V]
  OP.a = VARIABLE
  OP.b = ARR

```

**examples**
```
coming soon :)
```

##5. GOTO instruction
This instruction is of the form 
```
GOTO x
```
where **x** (the only operand) can be of type **C or V or Arr[v]**

This is a 32 bit instruction.

**opcode**
```c
0x15 (21)
```
**operation**
```
Executing this command will cause the IP (instruction pointer) to jump to instruction number "x"
```
**encoding**
```
OPCODE, OP_type, OP.b, OP.a

since the operand can be any of three types, the OP_type will tell us the 
type of "x" in the instruction.

The OP_type is encoded as follows

00  00 - 0000
\/  \       /
x    unused

bit value to type of OP ("x") mapping :
00 => OP is of type 'C'
  OP.a = Const
  OP.b = unused
  
01 => OP is of type 'V'
  OP.a = VARIABLE
  OP.b = unused
  
10 => OP is of type Arr[V]
  OP.a = VARIABLE
  OP.b = ARR

```

**examples**
```
coming soon :)
```
##6. GET instruction
This instruction is of the form 
```
GET x
```
where **x** (the only operand) can be of type **C or V or Arr[v]**

This is a 32 bit instruction.

**opcode**
```c
0x16 (22)
```
**operation**
```
Executing this command will return the value of "x"
```
**encoding**
```
OPCODE, OP_type, OP.b, OP.a

since the operand can be any of three types, the OP_type will tell us the 
type of "x" in the instruction.

The OP_type is encoded as follows

00  00 - 0000
\/  \       /
x    unused

bit value to type of OP ("x") mapping :
00 => OP is of type 'C'
  OP.a = Const
  OP.b = unused
  
01 => OP is of type 'V'
  OP.a = VARIABLE
  OP.b = unused
  
10 => OP is of type Arr[V]
  OP.a = VARIABLE
  OP.b = ARR

```

**examples**
```
coming soon :)
```

##7. ARITHMETIC and BIT instructions
This instruction is of the form 
```
INS x, y
```
where **INS** is a arithmetic or bitwise operation command;
**x** (operand1) and **y** (operand2) can be of type **C or V or Arr[v]**

There are **32 bit** and **64 bit** variations for each instruction.

**opcode**
> Each **INST** uses 2 opcodes (one for 32 bit encoding, another for 64 bit) based on what the **x**, **y** values are. 
  opcode starts from 48 (0011-0000), ends at 79 (0100-1111). Bit excoding is done based on this 
  map containing **INST** to the LS nibble value of the corresponding OPCODE.
  ```python
  opcode_dict = {
		#These inst own the (0011-XXXX) address space
					'ADD'	: 48,
					'SUB'	: 50,
					'MUL'	: 52,
					'DIV'	: 54, 
					'MOD'	: 56,
		#These inst own the (0100-XXXX) address space 
					'BSL'	: 64,
					'BSR'	: 66, 
					'AND'	: 68,
					'OR'	: 70,
					'NOT'	: 72
	}
  ```

###7.a ADD V, y

This is a 32 bit instruction where **y** can be a variable or constant.

**opcode**
```c
0x30 (48)
```

**operation**
```
Executing this command will add y to V and return the new value of V
```

**encoding**
```
OPCODE, OP_type, VARIABLE, OP

Byte3 : OPCODE    : 0011-0000   (OPCODE - Fixed)
Byte2 : OP_type   : (operand type - since the operands can be of any type, this indicates what the operands are)
Byte1 : VARIABLE  : (the address of variable V)
Byte0 : OP        : (reserved for operator 1, actual value depends on OP_decider)

since the operand can be C or V, the OP_type will tell us the type of "y" in the instruction.
The OP_type (8bits) is encoded as follows :
1000 - 0000
 |
 y

if bit 6 == 0
  type(y) = C
  OP -> C
  
if bit 6 == 1
  type(y) = VARIABLE
  OP -> address of variable "y"
```

**examples**
```
coming soon :)
```

###7.b ADD x, y

This is a **64 bit** instruction where either **x** or  **y** is of type Arr[V]
Note that **x** can't be of type C, while **y** can be of any type.

**opcode**
```c
0x31 (49)
```

**operation**
```
Executing this command will add y to x and return the new value of x
```

**encoding**
```
OPCODE, OP_type , OP1.b, OP1.a
unused, unused  , OP2.b, OP2.a

(OP1 : x, OP2 : y)

Byte3 : OPCODE    : 0011-0001   (OPCODE - Fixed)
Byte2 : OP_type   : (operand type - since the operands can be of any type, this indicates what the operands are)
Byte1 : OP1.b     : (reserved for operator 1, actual value depends on OP_decider)
Byte0 : OP1.a     : (reserved for operator 1, actual value depends on OP_decider)

Byte4 : OP2.b     : (reserved for operator 2, actual value depends on OP_decider)
Byte5 : OP2.a     : (reserved for operator 2, actual value depends on OP_decider)
Byte6 : unused    : unused
Byte7 : unused    : unused


OP_Type (8 bits)

00  00  - 0000
\ / \ /   \  /
 x   y    unused
 
  bit 7,6 : type of operand x
  bit 5,4 : type of operand y

bit encoding that indicate what the corresponding operators are :
(and what they represent in the encoding in each case)

00 => C (Constant)
  OP.a : Const-lo
  OP.b : Const-hi
  
01 => V (Variable)
  OP.a : Variable address of V
  OP.b : unused
  
10 => Arr[V] 
  OP.a : Variable address of V
  OP.b : Array address of Arr.
  
(add a simple example here)
```

**examples**
```
coming soon :)
```

###7.x SUB, MUL, DIV, OR, AND, BSL/R, NOT instructions

These instructions are similar to **ADD** discussed above. Each have two versions - 32bit and 64 bit version. 
The opcode for the 32 bit version has been mentioned at the begining of section 7, the OPCODE for 64 bit is opcode for 32 bit + 1
The encoding pattern is the same, only thing that changes is the pattern
**operation**
```
SUB x, y : x = x - y,  return x
MUL x, y : x = x * y,  return x
DIV x, y : x = x / y,  return x
AND x, y : x = x & y,  return x
OR  x, y : x = x | y,  return x
BSL x, y : x = x >> y, return x
BSR x, y : x = x << y, return x
NOT x, y : x = ~y,     return x
```

##8. HALT instruction

Simple instruction with no operands

**opcode**
```c
0x7F (127)
```

**operation**
```
Executing this instruction halts an ongoing BotSpeak script in the PRU. 
If no script is running, this instruction does nothing.
```

**encoding**
```
OPCODE, unused, unused, unused

Byte3   : OPCODE : 0111-1111 (fixed)
Byte2-0 : unused : unused
```

**examples**
```
coming soon
```

##9. ABORT, DEBUG, RUN, (END)SCRIPT instructions

These instructions are known as **control instructions**. They are not run on the PRU, rather are interpreted on the Linux side. (userspace python lib for now, may later be moved to the kernel)

**opcode**
```c
None - it is not compiled, 
rather the compiler framework itself interprets the cmd and does the needful.
```

**operation**
```
ABORT	: kills the current script, all variables and arrays defined are freed.
DEBUG	: streams the PRU Speak variables and state to the userspace. 
SCRIPT	: marks the starting of a script.
ENDSCRIPT : marks the ending of a script.
RUN	: runs the most recently defined script.
```

**encoding**
```
NO enccoding
```

**examples**
```
coming soon
```

