/*
 Proyect: SCADAcar
 Developed by: Ruy García
 Created: July 22, 2014
 By: Ruy García
 */
 
#include <SoftwareSerial.h>

SoftwareSerial bt(5,4);
char btt = '0';
char srl = '0';

void setup()
{
  Serial.begin(115200);
  bt.begin(9600);
}

void loop()
{
  sendMessage("message of LOG test");
  delay(1000);
}

void sendMessage(String message)
{
  Serial.print("Sending message: ");
  Serial.println(message);
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
