import pru_speak
import socket

def handle_data(bs_code):
        '''
        handles the BS commands which are part of the incoming packet
        seperately takes care of single instructions and scripts
        bs_code (str) : TCP packet data. The code block with BS instructions seperated by new lines.
        '''
        #print "bs_code.strip() " , bs_code.strip()
        bs_code = bs_code.strip().split("\n")
        bs_code = map(str.strip, bs_code)
        print "bs_code \n", bs_code
        # change logic either here or in compiler.py to make 
        # all this more clean and efficient. but this is ok for now.

        if "SCRIPT" in bs_code:
                #download and keep the script
                start = bs_code.index("SCRIPT")
                try:
                        end = bs_code.index("ENDSCRIPT")
                        #get the script postion
                        script_list = bs_code[start+1 : end]
                        #remove it out of the original list of inst.
                        bs_code = bs_code[:start] + bs_code[end+1 :]

                        pru_speak.SCRIPT_CODE = '\n'.join(script_list)
                        print "SCRIPT loaded\n",  pru_speak.SCRIPT_CODE + "\n ENDSCRIPT \n"
                except Exception as e:
			print "ENDSCRIPT NOT FOUND"
                        return -1 #ENDSCRIPT not found - syntax error

        for inst in bs_code :
                if inst == "RUN": #have to move run to PRU, why simply put it here?
                        if pru_speak.SCRIPT_CODE :
                                #maybe abort the previous script before this?
                                pru_speak.load(pru_speak.SCRIPT_CODE, trigger = True)
                        else :
                                return -1
                else :
                        pru_speak.single_instruction(inst)

        return 1

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

while 1:
	try:
		#blocking wait for data from client
		bs_code = conn.recv(BUFFER_SIZE)
		print "recieved\n", bs_code, "\n"
		#print type(bs_code)
		if not bs_code: break #why did I keep this? would I ever want to break out of the loop?
		
		#have to take care of return values later!
		if handle_data(bs_code) == 1: #does all the single_instruction, load, execute etc
			conn.send('ok\n')
                        print "ok"
		else:
			conn.send('syntax error')
                        print "syntax error"

	except Exception as e:
		print "error : ", e
		break

conn.close()

