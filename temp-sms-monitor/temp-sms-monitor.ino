#include <OneWire.h>
#include <DallasTemperature.h>

#include "SoftwareSerial.h"

// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);  

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

#define NUMBER_STR "0703725262"

String cmd = "";

SoftwareSerial mySerial(2, 3);

void sendSMS(){
  mySerial.println("AT+CMGF=1");
  delay(500);
  mySerial.print("AT+CMGS=\"");
  mySerial.print(NUMBER_STR);
  mySerial.print("\"\r\n");
  delay(500);
  mySerial.print("Yo!");
  delay(500);
  mySerial.write(26);
}

void updateSerial()
{
  while (Serial.available()) 
  {

    cmd+=(char)Serial.read();
 
    if(cmd!=""){
      cmd.trim();  // Remove added LF in transmit
      Serial.println("got S");

      if (cmd.equals("S")) {
        sendSMS();
      } else {
        mySerial.print(cmd);
        mySerial.println("");
      }
    }
  }
  
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Initializing...");

  sensors.begin();

  mySerial.begin(115200);

  delay(5000);

  mySerial.println("AT");                 // Sends an ATTENTION command, reply should be OK
  updateSerial();
//  mySerial.println("AT+CMGF=1");          // Configuration for sending SMS
//  updateSerial();
//  mySerial.println("AT+CNMI=1,2,0,0,0");  // Configuration for receiving SMS
//  updateSerial();
}

void loop(void)
{ 
  // Send the command to get temperatures
  sensors.requestTemperatures(); 
  float tempValue = sensors.getTempCByIndex(0);
  //print the temperature in Celsius
  Serial.print("Temperature: ");
  Serial.print(tempValue);
  Serial.print("C\n");
  
  updateSerial();
  delay(1000);
}
