#!/usr/bin/env python
from pru_speak import pru_speak
import socket 
#Server on local host @ port 6060
TCP_IP = '127.0.0.1'
TCP_PORT = 6060

#The max size upto which is recieved at one go
BUFFER_SIZE = 1024 * 2  

#create a passive socket and bind it to 127.0.0.1:6060
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))

#Take max of one connection
s.listen(1)
print "PRU Speak server at ", TCP_IP + ":" + str(TCP_PORT)

#connection is a one time affair - handles only one client
conn, addr = s.accept()
print 'Connection address:', addr

while True:
        try:
                #blocking wait for data from client
                bs_code = conn.recv(BUFFER_SIZE)
                print "recieved : \n", bs_code, "\n"

                if (not bs_code) or (bs_code == 'EXIT'):
			break #send empty data to shutdown server

		try:
                	ret = pru_speak.execute_instruction(bs_code)
			print "Return value : ", ret
			if ret == []:
				#no return value
				conn.send('\n')
			else:
				#the sends the list of ret values as a string sperated by " "
	        	        conn.send(" ".join(map(str, ret)) + "\n")

	        except Exception as e:
                	print e


        except Exception as e:
                print "error : ", e
                break

conn.close()
