/*
 Proyect: SCADAcar
 Developed by: Ruy García
 Created: July 22, 2014
 By: Ruy García
 */

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// Bluetooth Serial parameters:
SoftwareSerial bt(5,4);
char btt = '0';
char srl = '0';

// TinyGPS parameters:
static const int RXPin = 2, TXPin = 3;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps; // The TinyGPS++ object
SoftwareSerial ss(RXPin, TXPin);

int readCount = 0;
int delayTime = 10; // delay time (in seconds) between messages.

void setup()
{
  Serial.begin(115200);
  bt.begin(9600);
  ss.begin(GPSBaud);
}

void loop()
{
  readGPS();
}

void sendMessage(String message)
{
  //Serial.print("Sending message: ");
  //Serial.println(message);
  bt.println(message);
}

void readMessage()
{
  if (bt.available())
  {
    String command; //string to store entire command line
    while (bt.available())
    {
      btt = bt.read();
      Serial.print(btt);
      delay(50);
    }
  }
}

void readGPS()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}

void displayInfo()
{
  String strValue;
  char charBuf[15];
  if (gps.location.isValid())
  {
    // This function is thanks to http://dereenigne.org/arduino/arduino-float-to-string
    dtostrf(gps.location.lat(), 12, 6, charBuf);
    strValue += charBuf;
    //Serial.print(strValue);
    strValue += ",";
    //Serial.print(F(","));
    dtostrf(gps.location.lng(), 12, 6, charBuf);
    strValue += charBuf;
    //Serial.print(charBuf);
  }
  else
  {
    //Serial.print(F("INVALID"));
    strValue += "INVALID";
  }

  //Serial.println();
  // This will execute the sending message function when we want suposing that
  // each second it reads the TinyGPS
  if(readCount++ % delayTime == 0) {
    sendMessage(strValue);
  }
}
