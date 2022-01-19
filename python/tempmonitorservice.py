#!/usr/bin/python
import os, time

# APPLICATION FOR MONITORING TEMPERATURE AND SENDING AN ALERT VIA SMS IF
# ANY TRIGGER IS TRIPPED


oneWireTempDevice="/sys/bus/w1/devices/28-041690b555ff/w1_slave"
reportPhoneNumber="0703725262"
tempLowerThreshold=20.0
tempHigherThreshold=25.0


def readTemperature():
    dev = os.open(oneWireTempDevice, os.O_RDONLY)

    stringVar = os.read(dev,100)

    temperatureStr = stringVar.split()[-1][2:]

    temperatureFloat = float(temperatureStr)/1000.0

    return temperatureFloat

def sendSms():
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
