import mmap
import os

filename = "/sys/bus/platform/drivers/pru_speak/4a300000.prurproc/pru_speak_binfile"

file = open(filename, "r+")
size = 4096

try:
	print file.fileno()
	data = mmap.mmap(file.fileno(), size)

except Exception as e:
	print  e

else :

        # basics
        print data
        print len(data), size

        # use slicing to read from the file
        print data[0]

