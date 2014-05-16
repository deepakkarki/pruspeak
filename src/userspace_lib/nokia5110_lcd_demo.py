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
SET next_op, 1
SET write, 0
SET inc_x, 0
SET inc_y, 0
SET end_x, 0
SET end_y, 0
SET setup_1, 0
SET setup_2, 0
SET setup_3, 0
SET setup_4, 0
SET setup_5, 0

SCRIPT
GOTO next_op

; setup
SET DIO[reset], 0
SET DIO[reset], 1
SET data_ncommand, 0
SET data, 0x21
SET next_op, setup_1
GOTO write
;LBL setup_1
SET data, 0xB1
SET next_op, setup_2
GOTO write
;LBL setup_2
SET data, 0x04
SET next_op, setup_3
GOTO write
;LBL setup_3
SET data, 0x15
SET next_op, setup_4
GOTO write
;LBL setup_4
SET data, 0x20
SET next_op, setup_5
GOTO write
;LBL setup_5
SET data, 0x0C
SET next_op, 0
GOTO write

; write
SET DIO[dc], data_ncommand
IF (data_ncommand != 1) GOTO inc_x
ADD last_x, 1
;LBL inc_x
IF (last_x != 84) GOTO inc_y
ADD last_y, 1
;LBL end_x
IF (last_x != 84) GOTO end_x
SET last_x, 0
;LBL end_x
IF (last_y != 48) GOTO end_y
SET last_y, 0
;LBL end_y
SET DIO[sce], 0
SET DIO[sclk], 1
SET DIO[sclk], 0
SET DIO[sce], 1
GOTO next_op

ENDSCRIPT
RUN
'''

from pru_speak import pru_speak

ret = pru_speak.execute_instruction(SETUP)
print ret
