import pru_speak
import socket

TCP_IP = '127.0.0.1'
TCP_PORT = 6060
BUFFER_SIZE = pru_speak.PAGE_SIZE * 2
MESSAGE = \
'''     SET DIO[0] , 1
        WAIT 1
        SET DIO[0], 0
        WAIT 1
        GOTO 0'''


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
s.send(MESSAGE)
data = s.recv(BUFFER_SIZE)
s.close()

print "received data:", data
