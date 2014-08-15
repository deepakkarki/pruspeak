import socket
import sys 
out = sys.stdout
sentinel = ''

TCP_IP = '127.0.0.1'
TCP_PORT = 6060
BUFFER_SIZE = 1024 * 2

def get_data():
	out.write("ps>") #prompt the user
	l = []
	
	for line in iter(raw_input, sentinel):
		l.append(line)		#get the input
		out.write("...")	
	
	return '\n'.join(l)	#return the data entered


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))

while 1:
	try:
		msg = get_data()
		print "sending message\n", msg, "\n"
		s.send(msg)
		data = s.recv(BUFFER_SIZE)
		if data != '\n':
			print "return value:", data

	except Exception as e:
		print e
		print "Ending Connection"
		s.close()
		break		
		

