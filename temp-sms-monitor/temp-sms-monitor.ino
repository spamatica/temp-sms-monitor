#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

#include "SoftwareSerial.h"

// constants
#define MODEM_ENABLE
//#define SMS_ENABLE
#define EEPROM_START 0
#define ONE_WIRE_BUS 4
#define MODEM_SPEED 9600
#define NUMBER_STR "0703725262"

const float UPPER_LIMIT = 30.0;                   // hi temp limit (celsius)
const float LOWER_LIMIT = 15.0;                   // lo temp limit (celsius)
const long REBOOT_INTERVAL = 3600 * 24 * 7;        // reboot interval: 7 days

enum Alerts
{
  NoError,
  HiTempError,
  LowTempError,
};

void(* resetFunc) (void) = 0;               //declare reset function @ address 0

// variables
unsigned long upTimeSeconds = 0;
bool hasLimit=false;

OneWire oneWire(ONE_WIRE_BUS);              // Setup a oneWire instance to communicate with any OneWire device
DallasTemperature sensors(&oneWire);        // Pass oneWire reference to DallasTemperature library
SoftwareSerial modemSerial(2, 3);

/////////////////////////////////////////////////
void sendSMS(char *message)
{
  Serial.print("SEND SMS: ");
  Serial.println(message);
  
#ifdef SMS_ENABLE
  modemSerial.print("AT+CMGS=\"");
  modemSerial.print(NUMBER_STR);
  modemSerial.print("\"\r\n");
  delay(500);
  modemSerial.print(message);
  delay(500);
  modemSerial.write(26); // write Ctrl+Z to end message
#endif
}

/////////////////////////////////////////////////
void readModem()
{  
  while(modemSerial.available()) 
  {
    Serial.write(modemSerial.read());//Forward what Software Serial received to Serial Port
  }
}

/////////////////////////////////////////////////
int checkLimits(float temperatureValue)
{
  if (temperatureValue > UPPER_LIMIT)
  {
    Serial.print("UPPER LIMIT reached\n\r");
    return HiTempError;
  }
  else if (temperatureValue < LOWER_LIMIT)
  {
    Serial.print("LOWER LIMIT reached\n\r");
    return LowTempError;
  }

  return NoError;
}

/////////////////////////////////////////////////
void upTimeToString(char *outString, unsigned long seconds)
{
    int hour = seconds / (3600);
  int minute = (seconds - hour * 3600) /60;
  int second = (seconds - hour * 3600) - minute * 60;

  sprintf(outString, "%dh %dm %ds ", hour, minute, second);
}

/////////////////////////////////////////////////
void writeTimeToEEPROM(unsigned long currentTime)
{
  // write currentTime (in seconds) to four bytes
  EEPROM.write(EEPROM_START    ,  currentTime & 0x000000FF);
  EEPROM.write(EEPROM_START + 1, (currentTime & 0x0000FF00) >> 8);
  EEPROM.write(EEPROM_START + 2, (currentTime & 0x00FF0000) >> 16);
  EEPROM.write(EEPROM_START + 3, (currentTime & 0xFF000000) >> 24);
}

/////////////////////////////////////////////////
unsigned long readTimeFromEEPROM()
{
  unsigned long returnValue = 0;
  unsigned long v1 = EEPROM.read(EEPROM_START);
  unsigned long v2 = EEPROM.read(EEPROM_START + 1);
  unsigned long v3 = EEPROM.read(EEPROM_START + 2);
  unsigned long v4 = EEPROM.read(EEPROM_START + 3);

//  Serial.println(v1);
//  Serial.println(v2);
//  Serial.println(v3);
//  Serial.println(v4);
  
  return long(v1) + long(v2 << 8) + long(v3 << 16) + long(v4 << 24);
}

//
/////////////////////////////////////////////////
void initModem()
{
#ifdef MODEM_ENABLE
  modemSerial.begin(MODEM_SPEED);
  delay(5000);
  modemSerial.println("AT");                 // Sends an ATTENTION command, reply should be OK
  delay(200);
  readModem();
  #ifdef SMS_ENABLE
    modemSerial.println("AT+CMGF=1");
    delay(500);
    readModem();
  
  //  modemSerial.println("AT+CNMI=1,2,0,0,0");  // Configuration for receiving SMS
  //  delay(200);
  //  readModem();
  #endif
#endif  
}

//
/////////////////////////////////////////////////
void setup(void)
{
  char startupMessage[40];

  Serial.begin(9600); // init console
  Serial.println("Initializing...");


  sensors.begin();
  initModem();

  // report uptime
  unsigned long lastUptime = readTimeFromEEPROM();
  char timeString[20];
  upTimeToString(timeString, lastUptime);
  sprintf(startupMessage, "OMSTART.\n\rSenast uptime %s\r\n%ld sekunder", timeString, lastUptime);
  sendSMS(startupMessage);

  // clear uptime to be able to detect unscheduled reboots
  writeTimeToEEPROM(0Ul);
}

//
/////////////////////////////////////////////////
void loop(void)
{ 
  char messageString[40];
  char temperatureAsString[5];
  char timeString[20];
  
  // Send the command to get temperatures
  sensors.requestTemperatures(); 
  float temperatureValue = sensors.getTempCByIndex(0);

  dtostrf(temperatureValue, 5, 2, temperatureAsString);
  //print the temperature in Celsius
  Serial.print("Temperature: ");
  Serial.print(temperatureAsString);
  Serial.print("C ");
  
  upTimeToString(timeString, upTimeSeconds);
  sprintf(messageString, "uptime %s", timeString);
  Serial.println(messageString);
  
  int currentLimit = checkLimits(temperatureValue);

  if (hasLimit == false && currentLimit != NoError)
  {
    // entering an alert state
    if (currentLimit == HiTempError)
    {
      sprintf(messageString, "HI temp %sC\n", temperatureAsString);
      sendSMS(messageString);
    }
    if (currentLimit == LowTempError)
    {
      sprintf(messageString, "LO temp %sC\n", temperatureAsString);
      sendSMS(messageString);
    }
      
    hasLimit = true;
  }
 
  if (hasLimit && currentLimit == 0)
  {
    // time to clear 
    hasLimit = false;
  }

#ifdef SMS_ENABLE
  modemSerial.println("AT");                 // Sends an ATTENTION command, reply should be OK
  delay(200);
  readModem();
#endif

  delay(2000);
  
  upTimeSeconds = millis() / 1000;            // current uptime

  if (upTimeSeconds > REBOOT_INTERVAL)
  {
    writeTimeToEEPROM(upTimeSeconds);
    Serial.println("time to reboot");
    delay(300);
    resetFunc();    
  }
}
