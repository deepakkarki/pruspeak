from mmap import mmap
from compiler import compile
import time, struct

PAGE_SIZE	=	4096
HOME		=	"/sys/devices/ocp.3/4a300000.prurproc/"
INIT_FILE	=	HOME + "pru_speak_shm_init"
EXEC_FILE	=	HOME + "pru_speak_execute"
MEM_OFF		=	None

def _mem_init( ):
	#read the starting address (physical) of shared memory, also pass it on to the PRU (via downcall)
	global MEM_OFF
	with open(INIT_FILE) as f:
		MEM_OFF = int(f.read(), 16) 


def load(code, trigger=False):
	'''
	loads the BotSpeak code into memory. Internally calls the compile module.
	'''
	#if the shared memory is not initialized
	if not MEM_OFF:
		_mem_init()
		
	#open /dev/mem and mmap it to access the physical memory through our virtual space.
	with open("/dev/mem", "r+b" ) as f:
	
		mem = mmap(f.fileno(), PAGE_SIZE, offset=MEM_OFF)
		#var mem contains the map if successful
		
		byte_code	=	compile(code)
		code_offset	=	0
		
		#write every 32 bit byte code into the shared memory 
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


