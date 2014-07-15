from mmap import mmap
import time, struct

PAGE_SIZE	=	4096
HOME		=	"/sys/devices/ocp.3/4a300000.prurproc/"
INIT_FILE	=	HOME + "pru_speak_shm_init"
EXEC_FILE	=	HOME + "pru_speak_execute"
SINGLE_INST	=	HOME + "pru_speak_single_cmd"
ABORT		=	HOME + "pru_speak_abort"
DEBUG		=	HOME + "pru_speak_debug"
MEM_OFF		=	None

def _mem_init( ):
	#read the starting address (physical) of shared memory, also pass it on to the PRU (via downcall)
	global MEM_OFF
	with open(INIT_FILE) as f:
		MEM_OFF = int(f.read(), 16) 


def load(byte_code, trigger=False):
	'''
	loads the BotSpeak *bytecode* code into memory. 
	byte_code is list of BS *bytecode* instructions
	'''
	#if the shared memory is not initialized
	if not MEM_OFF:
		_mem_init()

	#open /dev/mem and mmap it to access the physical memory through our virtual space.
	with open("/dev/mem", "r+b" ) as f:
	
		mem = mmap(f.fileno(), PAGE_SIZE, offset=MEM_OFF)
		#var mem contains the map if successful
		
		code_offset	=	0
		
		#write every 32 bit byte-code into the shared memory 
		for inst in byte_code:
			mem[code_offset : code_offset + 4] = struct.pack("<L", inst)
			code_offset += 4
			
	if trigger :
		execute()


def execute():
	'''
	Writing 1 to EXEC_FILE tells the PRU to start executing the script
	'''
	with open(EXEC_FILE, "w") as f:
		f.write('1')


def single_instruction(byte_code):
	'''
	get the PRU to execute an single instruction
	instruction (int/tuple containing 32bit / 2 32 bit byte-code inst) : single BotSpeak Instruction
	'''
	if type(byte_code) == tuple :
		# **WARNING** LSB will be written in first - take note while coding kernel driver
		to_write_low = struct.pack("<L", byte_code[0])
		to_write_high = struct.pack("<L", byte_code[1]) #kernel still does not support 64 bit stuff

		with open(SINGLE_INST, "w") as f:
			f.write(to_write_low + to_write_high)
			
	else :
		to_write = struct.pack("<L", byte_code) #kernel still does not support 64 bit stuff

		with open(SINGLE_INST, "w") as f:
			f.write(to_write)

		
