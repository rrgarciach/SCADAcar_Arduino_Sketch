#include <SoftwareSerial.h>

// Here's a bluetooth device connect to pins 6 and 5
 SoftwareSerial bt(8,7);
 char btt = '0';
 char srl = '0';
 

void setup()
{
 Serial.begin(9600);
 Serial.println("System Up: BLUETOOTH SLAVE");
 bt.begin(38400);
}

void loop()
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
   
 if (Serial.available())
 {
   String command; //string to store entire command line
   while (Serial.available())
   {
     srl = Serial.read();
     Serial.print(srl);
     bt.print(srl);
     delay(50);
     command += srl; //iterates char into string
   }
   if (command == "9800") //this compares catched string vs. expected command string
   {
     Serial.println("COMMAND!");
     bt.println("COMMAND!");
     bt.end();
     bt.begin(9600);
   }
     Serial.println("");
     bt.println("");
 }
}
