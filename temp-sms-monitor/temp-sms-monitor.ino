#include <OneWire.h>
#include <DallasTemperature.h>

#include "SoftwareSerial.h"

// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);  

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

SoftwareSerial modemSerial(2, 3);

// DEFINE constants
#define MODEM_SPEED 115200
#define NUMBER_STR "0703725262"
float UPPER_LIMIT = 30.0;
float LOWER_LIMIT = 15.0;

void sendSMS(String message)
{
  modemSerial.print("AT+CMGS=\"");
  modemSerial.print(NUMBER_STR);
  modemSerial.print("\"\r\n");
  delay(500);
  modemSerial.print(message);
  delay(500);
  modemSerial.write(26); // write Ctrl+Z to end message
}

void readModem()
{  
  while(modemSerial.available()) 
  {
    Serial.write(modemSerial.read());//Forward what Software Serial received to Serial Port
  }
}

void setup(void)
{
  Serial.begin(9600); // init console
  Serial.println("Initializing...");

  sensors.begin();

  modemSerial.begin(MODEM_SPEED);

  delay(5000);

  modemSerial.println("AT");                 // Sends an ATTENTION command, reply should be OK
  delay(200);
  readModem();
  
  modemSerial.println("AT+CMGF=1");
  delay(500);
  readModem();

//  modemSerial.println("AT+CNMI=1,2,0,0,0");  // Configuration for receiving SMS
//  delay(200);
//  readModem();
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

  if (tempValue > UPPER_LIMIT)
  {
    Serial.print("UPPER LIMIT reached\n\r");
  }
  else if (tempValue < LOWER_LIMIT)
  {
    Serial.print("LOWER LIMIT reached\n\r");
  }

  modemSerial.println("AT");                 // Sends an ATTENTION command, reply should be OK
  delay(200);
  readModem();
  
  delay(1000);
}
