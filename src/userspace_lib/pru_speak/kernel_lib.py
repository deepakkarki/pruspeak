from mmap import mmap
import time, struct

PAGE_SIZE	=	4096
HOME		=	"/sys/class/misc/pru_speak/"
INIT_FILE	=	HOME + "pru_speak_shm_init"
EXEC_FILE	=	HOME + "pru_speak_execute"
SINGLE_INST	=	HOME + "pru_speak_single_cmd"
SINGLE_INST_64	=	HOME + "pru_speak_single_cmd_64"
ABORT		=	HOME + "pru_speak_abort"
DEBUG		=	HOME + "pru_speak_debug"
STATUS		=	HOME + "pru_speak_status"
CODE_MEM_OFF	=	0
RET_MEM_OFF	=	0	#base address for ret value shm
ret_counter	=	1	#like a top of stack pointer

def _mem_init( ):
	#read the starting address (physical) of shared memory, also pass it on to the PRU (via downcall)
	global CODE_MEM_OFF, RET_MEM_OFF

	with open(INIT_FILE) as f:
		code, ret = f.read().split(',')
		CODE_MEM_OFF = int(code, 16)
		RET_MEM_OFF  = int(ret, 16)


def load(byte_code, trigger=False):
	'''
	loads the BotSpeak *bytecode* code into memory. 
	byte_code is list of BS *bytecode* instructions
	'''
	#if the shared memory is not initialized
	if not (CODE_MEM_OFF and RET_MEM_OFF):
		_mem_init()

	#open /dev/mem and mmap it to access the physical memory through our virtual space.
	with open("/dev/mem", "r+b" ) as f:
	
		code_shm = mmap(f.fileno(), PAGE_SIZE, offset=CODE_MEM_OFF)
		#var mem contains the map if successful
		
		code_offset	=	0
		
		#write every 32 bit byte-code into the shared memory 
		for inst in byte_code:
			code_shm[code_offset : code_offset + 4] = struct.pack("<L", inst)
			code_offset += 4
			
	if trigger :
		execute()


def execute():
	'''
	Writing 1 to EXEC_FILE tells the PRU to start executing the script
	'''
	with open(EXEC_FILE, "w") as f:
		f.write('1')

def _get_return_value():
        '''
        gets the return value of the last executed instruction
        '''
	global ret_counter

        if not RET_MEM_OFF:
		#_mem_init()
		print "error! return value not initialized"

	ret_value = 0

	with open("/dev/mem", "r+b") as f:
		ret_shm = mmap(f.fileno(), PAGE_SIZE, offset=RET_MEM_OFF)
		tos = ret_shm[0:4]
		tos = struct.unpack("<L", tos)[0]

		while ret_counter == tos :
			tos = ret_shm[0:4]
			tos = struct.unpack("<L", tos)[0]
			#print "waiting..."
			#wait till PRU gives a return value

		ret_value = ret_shm[ret_counter*4 : (ret_counter*4)+4]
		ret_value = struct.unpack("<L", ret_value)[0]
		ret_counter = (ret_counter % 1023) + 1

	return ret_value 


def single_instruction(byte_code):
	'''
	get the PRU to execute an single instruction
	instruction (int/tuple containing 32bit / 2 32 bit byte-code inst) : single BotSpeak Instruction
	'''
	if not RET_MEM_OFF:
		_mem_init()

	if type(byte_code) == tuple :
		# **WARNING** LSB will be written in first - take note while coding kernel driver
		to_write_low = struct.pack("<L", byte_code[0])
		to_write_high = struct.pack("<L", byte_code[1]) #kernel still does not support 64 bit stuff
		#print "len of write : ", len(to_write_low + to_write_high)		
		with open(SINGLE_INST_64, "w") as f:
			f.write(to_write_low + to_write_high)
			
	else :
		to_write = struct.pack("<L", byte_code) 

		with open(SINGLE_INST, "w") as f:
			f.write(to_write)

	return _get_return_value()

		
