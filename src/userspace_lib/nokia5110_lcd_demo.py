SETUP = '''\
SET sclk, 0
SET sdin, 1
SET dc, 2
SET reset, 3
SET sce, 5
SET data_ncommand, 0
SET last_x, 0
SET last_y, 0
SET data, 0
SET next_op, 0
SET delay, 1

SCRIPT
SET DIO[4], 0
GOTO next_op

LBL write
SET DIO[dc], data_ncommand
SET DIO[sce], 0
SET DIO[sdin], 1
SET i, 0x80
AND i, data
IF (i == 0x80) GOTO bit7_high
SET DIO[sdin], 0
LBL bit7_high
WAIT delay
SET DIO[sclk], 1
WAIT delay
SET DIO[sclk], 0
SET DIO[sdin], 1
SET i, 0x40
AND i, data
IF (i == 0x40) GOTO bit6_high
SET DIO[sdin], 0
LBL bit6_high
WAIT delay
SET DIO[sclk], 1
WAIT delay
SET DIO[sclk], 0
SET DIO[sdin], 1
SET i, 0x20
AND i, data
IF (i == 0x20) GOTO bit5_high
SET DIO[sdin], 0
LBL bit5_high
WAIT delay
SET DIO[sclk], 1
WAIT delay
SET DIO[sclk], 0
SET DIO[sdin], 1
SET i, 0x10
AND i, data
IF (i == 0x10) GOTO bit4_high
SET DIO[sdin], 0
LBL bit4_high
WAIT delay
SET DIO[sclk], 1
WAIT delay
SET DIO[sclk], 0
SET DIO[sdin], 1
SET i, 0x08
AND i, data
IF (i == 0x08) GOTO bit3_high
SET DIO[sdin], 0
LBL bit3_high
WAIT delay
SET DIO[sclk], 1
WAIT delay
SET DIO[sclk], 0
SET DIO[sdin], 1
SET i, 0x04
AND i, data
IF (i == 0x04) GOTO bit2_high
SET DIO[sdin], 0
LBL bit2_high
WAIT delay
SET DIO[sclk], 1
WAIT delay
SET DIO[sclk], 0
SET DIO[sdin], 1
SET i, 0x02
AND i, data
IF (i == 0x02) GOTO bit1_high
SET DIO[sdin], 0
LBL bit1_high
WAIT delay
SET DIO[sclk], 1
WAIT delay
SET DIO[sclk], 0
SET DIO[sdin], 1
SET i, 0x01
AND i, data
IF (i == 0x01) GOTO bit0_high
SET DIO[sdin], 0
LBL bit0_high
WAIT delay
SET DIO[sclk], 1
WAIT delay
SET DIO[sclk], 0
SET DIO[sce], 1
IF (data_ncommand != 1) GOTO next_op
ADD last_x, 1
IF (last_x != 84) GOTO next_op
SET last_x, 0
ADD last_y, 1
IF (last_y != 48) GOTO next_op
SET last_y, 0
GOTO next_op

LBL setup
SET DIO[4], 1
SET return, next_op
SET DIO[reset], 0
WAIT 100
SET DIO[reset], 1
SET DIO[4], 1
WAIT delay
SET data_ncommand, 0
SET data, 0x21
SET next_op, setup_1
GOTO write
LBL setup_1
SET data, 0xB1
SET next_op, setup_2
GOTO write
LBL setup_2
SET data, 0x04
SET next_op, setup_3
GOTO write
LBL setup_3
SET data, 0x15
SET next_op, setup_4
GOTO write
LBL setup_4
SET data, 0x20
SET next_op, setup_5
GOTO write
LBL setup_5
SET data, 0x0C
SET next_op, return
GOTO write

LBL write_a
SET DIO[4], 1
SET return, next_op
SET data_ncommand, 1
SET data, 0
SET next_op, write_a_1
GOTO write
LBL write_a_1
SET data, 0x20
SET next_op, write_a_2
GOTO write
LBL write_a_2
SET data, 0x54
SET next_op, write_a_3
GOTO write
LBL write_a_3
SET data, 0x54
SET next_op, write_a_4
GOTO write
LBL write_a_4
SET data, 0x54
SET next_op, write_a_5
GOTO write
LBL write_a_5
SET data, 0x78
SET next_op, write_a_6
GOTO write
LBL write_a_6
SET data, 0x00
SET next_op, return
GOTO write

LBL main
SET DIO[4], 1
SET next_op, main_1
GOTO setup
LBL main_1
WAIT 100
SET next_op, repeat
GOTO write_a
LBL repeat
SET DIO[4], 0
WAIT 1000
GOTO main

ENDSCRIPT
RUN

WAIT 500
SET next_op, main
'''

from pru_speak import pru_speak

ret = pru_speak.execute_instruction(SETUP)
print ret
