# temp-sms-monitor
Some stuff for a temperature monitor using Orange PI, serial monitor and a modem to send SMS

Example reading temperature by hand:<br>
ddskrjo@orangepizero:~/temp-sms-monitor$ cat /sys/bus/w1/devices/28-041690b555ff/w1_slave<br>
7f 01 4b 46 7f ff 0c 10 ba : crc=ba YES<br>
7f 01 4b 46 7f ff 0c 10 ba t=23937<br>

Example sending SMS by hand to /dev/ttyUSB2:<br>
AT+CMGF=1<br>
AT+CMGS="070xxxxxxx",129<br>
Detta är ett meddelande<br>
CTRL+Z<br>


