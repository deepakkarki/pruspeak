from pru_speak import pru_speak
import os.path
import os

PATH = "/dev/pruspeak"

if os.path.exists(PATH):
	os.remove(PATH)

os.mkfifo(PATH)
f_read = open(PATH, "r")

while True:
	#blocking wait for data from client
	bs_code = f_read.read()

	if not bs_code:
		continue

        try:
                ret = pru_speak.execute_instruction(bs_code)
                print ret 
		#move this to 'cat' implementation later on. may need another thread for this.

        except Exception as e:
                print e


f_read.close()
os.remove(PATH)

