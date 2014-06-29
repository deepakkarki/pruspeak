import pru_speak
#import compiler

botspeak_code	= \
'''	SET DIO[4], 1
	WAIT 1000
	SET DIO[4], 0
	WAIT 1000
	GOTO 0'''

#print map(lambda x : hex(x), compiler.compile(botspeak_code))
pru_speak.load(botspeak_code)
pru_speak.execute()
pru_speak.single_instruction("SET DIO[0], 1")
