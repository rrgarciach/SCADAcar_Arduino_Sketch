#include <SoftwareSerial.h>

//===== CONFIG VALUES =====
int rx = 8;
int tx = 7;
int pinKey = 9;
int role = 1; // 0 = slave  |  1 = master
String name = "SCADAcar";
String pswd = "0258";
String address = "1122,33,DDEEFF";
int baudRate = 9600;

// Here's a bluetooth device connect to pins 6 and 5
 SoftwareSerial bt(rx,tx);
 char btt = '0';
 char srl = '0';
 

void setup()
{
 Serial.begin(9600);
 Serial.println("System Up: BLUETOOTH SLAVE");
 bt.begin(9600);
 //SLAVE CONFIG
  pinMode(pinKey, OUTPUT);
  digitalWrite(pinKey, HIGH);
  delay(5000);
  bt.println("AT"); //start AT commands
  delay(1000);
  bt.print("AT+ROLE="); bt.println(role); //set device as slave
  delay(1000);
  if (role == 0) {
    bt.print("AT+NAME="); bt.println(name);  //set device name
    delay(1000);
    bt.print("AT+PSWD="); bt.println(pswd); //set device name
    delay(1000);
  } else {
    bt.print("AT+CMODE=0"); //send CMODE=0, set connection mode to specific address
    delay(1000); 
    bt.print("AT+BIND="); bt.println(address); //send BIND=??, bind HC-05 to OBD bluetooth address
    delay(1000); 
    bt.print("AT+INIT"); //send INIT, cant connect without this cmd 
    delay(1000); 
    bt.print("AT+PAIR="); bt.print(address); bt.println(",20"); //send PAIR, pair with OBD address
    delay(1000);  
    bt.print("AT+LINK="); bt.println(address); //send LINK, link with OBD address
    delay(1000);
    bt.print("AT+UART="); bt.println(baudRate); //set baudrate
    delay(1000);
  }
  bt.println("AT+NAME?"); //get device name
  bt.println("AT+ADDR?"); //get device address (for connection)
  bt.println("AT+VERSION?"); //get device version
  digitalWrite(pinKey, LOW);
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
   if (command == "test") //this compares catched string vs. expected command string
   {
     Serial.print("COMMAND!");
     bt.print("COMMAND!");
   }
     Serial.println("");
     bt.println("");
 }
}
