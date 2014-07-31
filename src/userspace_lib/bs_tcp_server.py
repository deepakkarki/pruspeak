from pru_speak import pru_speak

#Server on local host @ port 6060
TCP_IP = '127.0.0.1'
TCP_PORT = 6060

#The max size upto which is recieved at one go
BUFFER_SIZE = pru_speak.PAGE_SIZE * 2  # Normally 1024, but we want fast response

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
                print "recieved\n", bs_code, "\n"

                #print type(bs_code)

                if (not bs_code) or (bs_code == 'EXIT'):
			break #send empty data to shutdown server

		try:
                	ret = pru_speak.execute_instruction(code)
        	        print ret #have to send this back 
					#-- what if client does not recieve?? :(

	        except Exception as e:
                	print e

                conn.send('syntax error')
		print "recieved code fragment : \n", bs_code

        except Exception as e:
                print "error : ", e
                break

conn.close()
