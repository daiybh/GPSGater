import socket
import time
import binascii
import threading


class SocketThread(threading.Thread):
    def __init__(self,port,sendStr,bhex=False):
        threading.Thread.__init__(self)
        self.port = port
        self.sendStr=sendStr
        self.bhex=bhex
        self.bHold = True

    def printError(self,errMsg):
        print("Error:",self.port,errMsg)
    def run(self):
        sock2=0
        bConnect=False
        while True:
            print("SocketThread")
            
            if sock2==0:
                sock2 = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

            bException=False    
            try:
                #sock2.connect(("192.168.200.102",self.port))
                sock2.connect(("127.0.0.1",self.port))
                bConnect = True

            
                if self.bhex:
                    sock2.send(binascii.a2b_hex(self.sendStr))
                else:
                    sock2.send(self.sendStr)
                #
                sock2.settimeout(10)
                recv = sock2.recv(4096)
                print('[recv',self.port,recv)

            except socket.timeout:
                self.printError("timeout")
                bException=True
            except Exception as ex:
                self.printError(ex)
                bException=True

            if bException:
                bConnect=False
                sock2.close()
                sock2=0
                time.sleep(1)
                continue
            
            if self.bHold==False:
                sock2.close()
                sock2=0
            
            time.sleep(1)





port =110
sendStr ="7e01000023015728572436000e00111111373034303444522d3230413120526f706530303101d4c14147323031310000517e"

#SendSocket(port,sendStr,True)

a = SocketThread(port,sendStr,True)
a.start()

port =120
sendStr ="<directtogps_command><deviceid>015728572436</deviceid><toBin>1</toBin><command>7e8001000501572857243600010001010200bc7e0000</command></directtogps_command>"

#SendSocket(port,sendStr)
b = SocketThread(port,sendStr)
b.start()


