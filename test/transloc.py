import string
def tranloc(ddMMmmm):
    pass

def getloc(hexData):
    return (hexData/1000000.0)    

def getlocA(lat,lng,label=""):
    print(label,getloc(lat),getloc(lng))

##getlocA(0x017e253d,0x061f280b,"Left")
##getlocA(0x017E19f7,0x061f32d0,"righ")
##getlocA(0x017E26c3,0x061f2597,"cent")



str2="7e8602001801478784641800000001000000010008017d02253d061f280b017d0219f7061f32d09a7e"
str1="7e0200002a01478784641800550000000000000013017d0226c3061f2597078b000000001311041439120104000017740202000003020000b47e"
str1="7e0200002a01478784641804500000000000000013017d0226c3061f2597078b000000001311041853400104000017750202000003020000807e"
def getData(srcString):
    x=''
    i=0
    #for  w in srcString:
    b7d=False
    for i in range(0,len(srcString),2):
        w = srcString[i:i+2]           
        if w!='7d':
            if not b7d:
                x+=w
            else:
                x+="{0:x}".format(int(w,16)+0x7d-1)
                b7d=False
            
        else:
            b7d=True
                
        if len(x)>7:
            break
    #print("getdata",i,x)    
    return i+2,int(x,16)

def doGetloc_8602(src):
    lat1,pos=doGetLoc(src)
    lat2,pos=doGetLoc(src[pos:])
    return 2,lat1,lat2

def doGetLoc(src):
    #print(src)
    pos1,lat = getData(src)    
    pos2,lng = getData(src[pos1:])
    return (getloc(lat),getloc(lng)),pos1+pos2

def doGetloc_0200(src):
    return 1,doGetLoc(src)[0]
    pass

def getLocation(src):
    nPos = src.find("7e8602")
    
    if nPos>-1:
        #print("8602",     doGetloc_8602(src[42:] ))
        return doGetloc_8602(src[42:])
        
    else:
        nPos = src.find("7e0200")
        if nPos >-1:
            #print("0200",            doGetloc_0200(src[42:]))
            return doGetloc_0200(src[42:])


ret1 =getLocation(str1)

ret2=""
ret2=getLocation(str2)
print("ret1",ret1)
print("ret2",ret2)

class point :
    def __init__(self,lat=0,lng=0):
        self.lat=lat
        self.lng=lng
        
    def out(self):
        return self.lat,self.lng
    def compare(self,p):
        if self.lat > p.lat:
            print("lat>")
        else:
            print("lat<")
            
        if self.lng > p.lng:
            print("lng>")
        else:
            print("lng<")
            
pleft = point()
pright = point()
pcur = point()


if len(ret1)>1 and ret1[0]==1 :
    
    #print( (ret1[1][0]))
    pcur.lat= float(ret1[1][0])
    pcur.lng= float(ret1[1][1])

if len(ret2)>2 and ret2[0]==2:
    pleft.lat= float(ret2[1][0])
    pleft.lng= float(ret2[1][1])
    
    pright.lat= float(ret2[2][0])
    pright.lng= float(ret2[2][1])

print("left",pleft.out())
print('right',pright.out())
print('cur',pcur.out())
def checkLoc():
    bOK=True
    if pcur.lat > pleft.lat :
        print("error pcur.lat[{0}]> pleft.lat[{1}]".format(pcur.lat,pleft.lat))
        bOK=False
    
    if pcur.lat < pright.lat:
        print("error pcur.lat[{0}] < pright.lat[{1}]".format(pcur.lat,pright.lat))
        bOK=False
    
    if pcur.lng < pleft.lng :
        print("err  pcur.lng[{0}]  < pleft.lng[{1}]".format(pcur.lng,pleft.lng))
        bOK=False
    
    if pcur.lng > pright.lng:
        print("err  pcur.lng[{0}]  > pright.lng[{1}]".format(pcur.lng,pright.lng))
        bOK=False
    if bOK:    print("pcur in pleft and pright")

checkLoc()

    



