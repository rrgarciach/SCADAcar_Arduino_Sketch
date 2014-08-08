#include <SoftwareSerial.h>
#define RxD 7                //Arduino pin connected to Tx of HC-05
#define TxD 8                //Arduino pin connected to Rx of HC-05

SoftwareSerial blueToothSerial(RxD,TxD); 

void setup()
{
 Serial.begin(38400); 
 blueToothSerial.begin(38400);
 pinMode(RxD, INPUT);
 pinMode(TxD, OUTPUT);
   
 } 
 
 void loop() {
  
  if (blueToothSerial.available()) {        //PORTA DO BLUETOOTH RX1 + TX1 (7 e 8)
    char inByte = blueToothSerial.read();
    Serial.write(inByte); 
   }
  

  if (Serial.available()) {   // Ler a porta Serial Arduino
    char inByte = Serial.read();
    blueToothSerial.print(inByte);   
  }
}
