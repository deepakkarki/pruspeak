from mmap import mmap
import time, struct

PAGE_SIZE = 4096
init_file = "/sys/devices/ocp.3/4a300000.prurproc/pru_speak_shm_init"
exec_file = "/sys/devices/ocp.3/4a300000.prurproc/pru_speak_execute"

with open(init_file) as f:
	mem_offset = int(f.read(), 16) #0x9f0c0000 #offest as of now should be got from dmesg
	

mem_size = PAGE_SIZE

with open("/dev/mem", "r+b" ) as f:
	mem = mmap(f.fileno(), mem_size, offset=mem_offset)
	packed_reg = mem[:4]
	reg_status = struct.unpack("<L", packed_reg)[0]
	print reg_status
	
	#HARD CODING - BAD
	
	DIO1 = 0x01000080  #DIO[0] = 1
	WAIT = 0x02000000 #WAIT 1
	DIO0 = 0x01000000  #DIO[0] = 0
	JUMP = 0x03000000 #JUMP to DIO1
	#['0x1000080', '0x2000001', '0x1000000', '0x2000001', '0x3000000']

	mem[0:4] = struct.pack("<L", DIO1)
	mem[4:8] = struct.pack("<L", WAIT)
	mem[8:12] = struct.pack("<L", DIO0)
	mem[12:16] = struct.pack("<L", WAIT)
	mem[16:20] = struct.pack("<L", JUMP)

with open(exec_file, "w") as f:
	f.write('1')

time.sleep(10)
'''
with open(exec_file, "w") as f:
        f.write('0')
'''
