AND_SCRIPT = '''\
SCRIPT
SET i, 0xAA
SET j, 0xF0
AND i, j
GET i
ENDSCRIPT
RUN
'''

SCRIPT = '''\
SET delay, 500
SET loop, flash
SCRIPT
GOTO loop

LBL flash
SET DIO[4], 1
WAIT delay
SET DIO[4], 0
WAIT delay
GOTO 0

LBL flash2
SET DIO[4], 1
WAIT delay
SET DIO[4], 0
WAIT delay
SET DIO[4], 1
WAIT delay
SET DIO[4], 0
WAIT delay
WAIT delay
GOTO 0

ENDSCRIPT
RUN
'''

RUN_FLASH = '''\
SET delay, 100
SET loop, flash
'''

RUN_DOUBLE_FLASH = '''\
SET loop, flash2
'''

from pru_speak import pru_speak
import time

print "Performing AND operation"
ret = pru_speak.execute_instruction(AND_SCRIPT)
print ret
time.sleep(5)

print "Starting flashing LED"
ret = pru_speak.execute_instruction(SCRIPT)
print ret
time.sleep(5)

print "Speeding up flashing LED"
ret = pru_speak.execute_instruction(RUN_FLASH)
print ret
time.sleep(5)

print "Switching to double-flash"
ret = pru_speak.execute_instruction(RUN_DOUBLE_FLASH)
print ret