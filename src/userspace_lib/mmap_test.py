from mmap import mmap
import time, struct

PAGE_SIZE = 4096
mem_offset = 0x9f0c0000 #offest as of now should be got from dmesg
mem_size = PAGE_SIZE

with open("/dev/mem", "r+b" ) as f:
	mem = mmap(f.fileno(), mem_size, offset=mem_offset)
	packed_reg = mem[:4]
	type(packed_reg)
	reg_status = struct.unpack("<L", packed_reg)[0]
	print reg_status

