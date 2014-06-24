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
sendStr="7e0102000701478783121305b7726f70656e7465827e"
#SendSocket(port,sendStr,True)
sendStr="7e0200003201478784641800110010000000000013017d022755061f25e207920000000013110116103801040000174402020000030200001206020000000100207e"

a = SocketThread(port,sendStr,True)
a.start()

port =120
sendStr ="<directtogps_command><deviceid>015728572436</deviceid><toBin>1</toBin><command>7e8001000501572857243600010001010200bc7e0000</command></directtogps_command>"
sendStr ="<?xml version=\"1.0\" encoding=\"utf-8\"?><setarea><id>43</id><sim>015728572436</sim><deviceid>015728572436</deviceid><type>2</type><leftlng>10242.0825504</leftlng><leftlat>2502.7253643999998</leftlat><rightlng>10242.7108308</rightlng><rightlat>2502.3567718</rightlat><alerttype>1</alerttype><areaid>1</areaid></setarea>"
sendStr ="<?xml version=\"1.0\" encoding=\"utf-8\"?><setarea><id>44</id><sim>014787831213</sim><deviceid>014787831213</deviceid><type>2</type><leftlng>10242.0516522</leftlng><leftlat>2502.767155</leftlat><rightlng>10242.896226</rightlng><rightlat>2502.3005818</rightlat><alerttype>1</alerttype><areaid>1</areaid></setarea>"
#sendStr ="<?xml version=\"1.0\" encoding=\"utf-8\"?><overspeed><maxspeed>2</maxspeed><minspeed>1</minspeed><countinue>10</countinue><deviceid>015728572436</deviceid></overspeed>"
#sendStr ="<?xml version=\"1.0\" encoding=\"utf-8\"?><setgprsparam><deviceid>015728572436</deviceid></setgprsparam>"


#SendSocket(port,sendStr)
b = SocketThread(port,sendStr)
b.start()


