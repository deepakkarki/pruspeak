#This is a shell to use pruspeak
#This will be later broken down into two versions 
#	- FIFO useable from the bash shell
#	- socket based server, client being LabView(?)

from pru_speak import pru_speak
import sys 
out = sys.stdout
sentinel = ''
def get_data():
	out.write("ps>") #prompt the user
	l = []

	for line in iter(raw_input, sentinel):
		l.append(line)		#get the input
		out.write("...")	

	return '\n'.join(l)	#return the data entered


while True:
	code = get_data()
	if not code:
		exit()
	try:
		ret = pru_speak.execute_instruction(code)
		print ret
	except Exception as e:
		print e

