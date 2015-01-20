#!/usr/bin/env python
#This is a shell to use pruspeak

from pru_speak import pru_speak
import sys 
out = sys.stdout
sentinel = ''
def get_data():
	out.write("ps>") #prompt the user
	l = []
	try:
		for line in iter(raw_input, sentinel):
			l.append(line)		#get the input
			out.write("...")	
	except KeyboardInterrupt as e:
		print "Closing terminal"
		return -1
	
	except EOFError as e:
		print "Closing terminal"
		return -1

	return '\n'.join(l)	#return the data entered


while True:
	code = get_data()
	if (code == -1) or (code == 'exit'):
		exit()

	if code == '':
		continue 

	try:
		ret = pru_speak.execute_instruction(code)
		print ret
	except Exception as e:
		print e

