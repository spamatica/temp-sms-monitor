
#!/usr/bin/python
import os,time

initStr = "AT+CMGF=1\n"
sendStr = "AT+CMGS=\"0703725262\",129\n"
message = "WOOOHOOOO\x1a"
dev = os.open("/dev/ttyUSB2", os.O_RDWR)
n=os.write(dev, initStr)
print "Sent init", n
time.sleep(1)
n=os.write(dev, sendStr)
print "Sent nbr", n
time.sleep(1)
n=os.write(dev, message)
print "Sent msg", n

os.close(dev)

#os.lseek(dev,0,os.SEEK_SET)
#print os.read(dev,100)


print "DOINE"
