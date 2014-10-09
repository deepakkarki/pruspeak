from pru_speak import pru_speak

#set pin 0 [pru0_r30_0] high for two second
EX1 = '''\
SET var1, 1
SET DIO[0], var1
'''

EX2 = '''\
SCRIPT
SET DIO[0], 1
WAIT 1000
SET DIO[0], 0
WAIT 1000
GOTO 0
ENDSCRIPT
RUN
'''
#triangle wave of frequence 0.5Hz
EX3 = '''\
SCRIPT
SET var1, 0
SET PWM[0], var1
WAIT 30
ADD var1, 1
IF (var1 < 100) GOTO 1
SET PWM[0], var1
WAIT 30
SUB var1, 1
IF (var1 > 0) GOTO 5
IF (1 > 0) GOTO 0
ENDSCRIPT
RUN
'''

ret = pru_speak.execute_instruction(EX3)
print ret

while True:
	pass

