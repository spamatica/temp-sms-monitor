#!/usr/bin/python
import os

#data = "xxxxxxxxxxxxxxxx"
dev = os.open("/sys/bus/w1/devices/28-041690b555ff/w1_slave", os.O_RDONLY)
#os.write(dev,data)
#os.lseek(dev,0,os.SEEK_SET)
print os.read(dev,100)

