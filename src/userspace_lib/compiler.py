import re


def f_set(cmd):
	'''
	as of now only handles DIO, should let is handle PWM etc
	'''
	def f_dio(pin, state):
		'''
		function handler for DIO. takes state (int) and pin (int)
		'''
		op = 0x01000000
		
		if pin > 16:
			raise Exception #bad hack

		op += pin

		if state:
			op = op | 0x80
		return op

	
	args = cmd[1].split(',')
	op_code = re.match('(DIO)\[(.)*\]' , args[0])
	
	if (op_code.group(1) == 'DIO') :
		return f_dio(int(op_code.group(2)), int(args[1]))


def f_wait(cmd):
	'''
	return the wait time
	'''
	op = 0x02000000
	op += int(cmd[1])
	return op

def f_goto(cmd):
	'''
	jump to address
	'''
	op = 0x03000000
	op += int(cmd[1])
	return op

CMD = {
        'SET'   : f_set,
        'WAIT'  : f_wait,
        'GOTO'  : f_goto
#       'LABEL' : f_label
}

def compile(code):
	'''
	takes in the code block, returns a list of BotSpeak bytecode
	code (str) : the code block with BS instructions seperated by new lines
	'''
	code = code.strip()
	#print "pgm :", pgm
	pgm = code.split('\n')
	byte_code = []
	
	for inst in pgm:
		inst = inst.strip()
		cmd = re.split('\s+', inst, 1) #max of one split
		print cmd
		byte_code.append( CMD[ cmd[0] ](cmd) ) 
		#cmd[0] contains the actual command eg: SET, WAIT, GOTO
		#cmd[1] contains rest of the args eg : "DIO[1] , 0" 
	
	return byte_code

if __name__ == '__main__':
	s = ''' SET DIO[0] , 1
		WAIT 1
		SET DIO[0], 0
		WAIT 1
		GOTO 0'''
	
	l = compile(s)
	print map( lambda x : hex(x), l)


