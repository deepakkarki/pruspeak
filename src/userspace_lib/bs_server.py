import pru_speak
import socket

TCP_IP = '127.0.0.1'
TCP_PORT = 6060
BUFFER_SIZE = pru_speak.PAGE_SIZE * 2  # Normally 1024, but we want fast response

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)

while 1:
	try:
		conn, addr = s.accept()
		print 'Connection address:', addr
		bs_code = conn.recv(BUFFER_SIZE)
		if not bs_code: break
		#print "received data:", bs_code
		pru_speak.load(bs_code)
		pru_speak.execute()
		conn.send('ok, executing script')  # echo
		conn.close()
	
	except Exception as e:
		print "error in connection"
	
	finally:
		conn.close()

