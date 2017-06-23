# temp-sms-monitor
Some stuff for a temperature monitor using Orange PI, serial monitor and a modem to send SMS

Example reading temperature by hand:
ddskrjo@orangepizero:~/temp-sms-monitor$ cat /sys/bus/w1/devices/28-041690b555ff/w1_slave 
7f 01 4b 46 7f ff 0c 10 ba : crc=ba YES
7f 01 4b 46 7f ff 0c 10 ba t=23937

Example sending SMS by hand to /dev/ttyUSB2:
AT+CMGF=1
AT+CMGS="070xxxxxxx",129
Detta är ett meddelande.
CTRL+Z

