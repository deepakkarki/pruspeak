#NOTE: This is a substitute for pruspeak.py in this new userspace interface
#This is the frontend userspace component of the interpreter
#The developer API interface exposed looks like this :
#	Userfrontend <-  Shell  <- this API
#	The API is mainly used to execute an BS instruction.

from bs_parse import parser
import kernel_lib

script_mode = False
script_code = []

def _get_return_value():
	'''
	gets the return value of the last executed instruction
	'''
	return 1 #for now. (I think this should be moved to kernel lib, though)
	
	
def str_to_list(code):
	'''
	converts a string of BS inst into a list
	'''
	#strip any leading/tailing '\n' or '  '
	code = code.strip()
	#split by newline
	code = code.split('\n')
	#strip each instruction, incase an element is '', then throw it out
	code = filter( lambda y : y != '', map(lambda x : x.strip(), code) )
	return code
	
	
def execute_instruction(cmd_set):
	'''
	Takes a list of BS instructions and executes them.
	returns the return value of each input inst. as a list
	
	cmd_set : either a list or a string; 
		if it is a str, it is internally converted to a list first
	'''
	global script_mode
	return_values = []
	if type(cmd_set) == str:
		cmd_set = str_to_list(cmd_set)
	
	for inst in cmd_set:
		if inst == 'SCRIPT':
			if not script_mode:
				script_mode = True
				script_code = [ ]
			#else Error
			else:
				return return_values.append(-1)
				
		elif inst == 'ENDSCRIPT':
			if script_mode:
				script_mode = False
				script_code.append(parser.parse('HALT'))
			else:
				return return_values.append(-1)
			
		elif inst == 'RUN':
			if script_code and (not script_mode) :
					kernel_lib.load(script_code)
					kernel_lib.execute()
			else :
				return return_values.append(-1)
	
		elif inst == 'DEBUG':
			with open(kernel_lib.DEBUG, "w") as f:
				f.write("1")
		
		elif inst == 'ABORT':
			with open(kernel_lib.ABORT, "w") as f:
				f.write("1")
		
		elif inst == 'SYSTEM':
			with open(kernel_lib.STATUS, "r") as f:
				print "Is PRU running the script : ", f.read() == '1'
	
		#from here, it has to be non-control instruction
		elif script_mode:
			#incase there is an ongoing SCRIPT being defined
			byte_code  = parser.parse(inst)
			
			if type(byte_code) == tuple:
			#64 bit instruction
				script_code.extend(byte_code)
				
			else:
			#32 bit instruction
				script_code.append(byte_code)
				
			
		else:
			#case of normal instruction
			byte_code = parser.parse(inst)
			kernel_lib.single_instruction(byte_code)
			return_values.append(_get_return_value())
			#byte_code = parser.parse(inst)
		
	return return_values
	
if __name__ == '__main__':
	s = '''
	SET var2, 0
	SET var3, 1
	SET arr1[0], 1
	SET arr1[1], 0
	SCRIPT
		SET DIO[0] , arr1[0]
		WAIT 1000
		SET DIO[0] , arr1[1]
		WAIT 1000
		IF (arr1[var2] > arr1[var3]) GOTO arr1[var3]
	ENDSCRIPT
	RUN
	'''
		
	print str_to_list(s)
	res = execute_instruction(["SET arr1[var2], 0"])
	print res
