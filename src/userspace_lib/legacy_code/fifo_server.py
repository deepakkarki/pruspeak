import pru_speak
import os

script = False 
pru_speak.SCRIPT_CODE = ''
def handle_inst_fifo(bs_code):
        '''
        handles the BS command that comes in one by one. 
	maintains state in case of SCRIPT
        '''
	global script
        bs_code = bs_code.strip() #only single BS inst
	print "bs_code : ", bs_code
	
	if bs_code == "ENDSCRIPT":
		if not script: #endscript without a startscript?
			return 0 #some problem
		script = False

	elif bs_code == "SCRIPT":
		if  script: #startscript in a script
			return 0
		script = True
	
	elif script:
		pru_speak.SCRIPT_CODE += '\n' + bs_code

	elif bs_code == "RUN":
		if pru_speak.SCRIPT_CODE :
			print "running script :"
			print pru_speak.SCRIPT_CODE
			pru_speak.load(pru_speak.SCRIPT_CODE, trigger = True)
		else : 
			return 0

	else :#normal single inst execution
		pru_speak.single_instruction(bs_code)

        return 1

os.mkfifo("/dev/pruspeak")
f_read = open("/dev/pruspeak", "r")

while 1:
	#blocking wait for data from client
	while 1:
		bs_code = f_read.read()
		if bs_code:
			print "handle_inst_fifo return value : ", handle_inst_fifo(bs_code)


f_read.close()

