import pru_speak
#import compiler

botspeak_code	= \
'''	SET DIO[0] , 1
	WAIT 1
	SET DIO[0], 0
	WAIT 1
	GOTO 0'''

#print map(lambda x : hex(x), compiler.compile(botspeak_code))
pru_speak.load(botspeak_code)
pru_speak.execute()

