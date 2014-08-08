 
//***********************************************//
//******Car Engine RPM and Shift Light***********//
//******with Arduino, HC-05 Bluetooth Module*****//
//**********and ELM-327 OBDII Bluetooth**********//
//***********************************************//
//**********Designed and Programmed**************//
//************by Kostas Kokoras******************//
//************kostas@kokoras.com*****************//

#include <Timer.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

#define RxD 7                //Arduino pin connected to Tx of HC-05
#define TxD 8                //Arduino pin connected to Rx of HC-05
#define Reset 9              //Arduino pin connected to Reset of HC-05 (reset with LOW)
#define PIO11 A2             //Arduino pin connected to PI011 of HC-05 (enter AT Mode with HIGH)
#define ledpin_green A0      //Arduino output pin for Shift Light Green led 
#define ledpin_yellow A1     //Arduino output pin for Shift Light Yellow led 
#define ledpin_red 13        //Arduino output pin for Shift Light Red led 
#define led_dec 10           //Arduino output pin for decades 7-segment commone anode display
#define led_mon 11           //Arduino output pin for monades 7-segment commone anode display
#define sel_sw 12            //Arduino input for storing curent Shift Light RPM
#define BT_CMD_RETRIES 5     //Number of retries for each Bluetooth AT command in case of not responde with OK
#define OBD_CMD_RETRIES 3    //Number of retries for each OBD command in case of not receive prompt '>' char
#define RPM_CMD_RETRIES 5    //Number of retries for RPM command
int BinaryPins[] = {3,4,5,6};//Arduino Pins connected to 74LS47 BCD-to-7-Segment
                  //A,B,C,D//  A is LSB

int addr=0;                  //EEPROM address for storing Shift Light RPM
unsigned int rpm,rpm_to_disp;//Variables for RPM
int shift_light_rpm;         //Variable for Shift Light RPM
unsigned int decades;        //Variable of RPM number decades for 7-seg disp
unsigned int monades;        //Variable of RPM number monades for 7-seg disp
boolean but_pressed_flag;    //Variable if RPM Shift Light button is pressed
boolean bt_error_flag;       //Variable for bluetooth connection error
boolean obd_error_flag;      //Variable for OBD connection error
boolean rpm_error_flag;      //Variable for RPM error
boolean rpm_retries;         //Variable for RPM cmd retries
int disp_bright;             //Variable for 7-seg disp brightness
                 
SoftwareSerial blueToothSerial(RxD,TxD);
Timer t;
                  
//-----------------------------------------------------//
void setup()
{
   
   pinMode(RxD, INPUT);
   pinMode(TxD, OUTPUT);
   pinMode(PIO11, OUTPUT);
   pinMode(Reset, OUTPUT);
   
   digitalWrite(PIO11, LOW);    //Set HC-05 to Com mode
   digitalWrite(Reset, HIGH);   //HC-05 no Reset
   
   pinMode(3, OUTPUT);   
   pinMode(4, OUTPUT);
   pinMode(5, OUTPUT);
   pinMode(6, OUTPUT);
 
   pinMode(ledpin_green, OUTPUT);
   pinMode(ledpin_yellow, OUTPUT);
   pinMode(ledpin_red, OUTPUT);
  
   pinMode(led_dec, OUTPUT);
   pinMode(led_mon,OUTPUT);
   
   pinMode(sel_sw,INPUT);
   
   //7-seg disp and shift light test
   demo();
   
   //Read Stored Shift Light RPM//
   shift_light_rpm=EEPROM.read(addr);                                  //Read EEPROM for stored value
   if ((shift_light_rpm<0) or (shift_light_rpm>99)) shift_light_rpm=0; //if value not correct set it to 0
   
   rpm_retries=0;
   but_pressed_flag=false;
   disp_bright=0;//Full Bright
 
   t.every(250,rpm_calc);//Every 250ms read RPM value from OBD
   
   //start Bluetooth Connection
   setupBlueToothConnection();
 
   //in case of Bluetoth connection error
   if (bt_error_flag){
     bt_err_flash();
   } 
 
   //OBDII initialitation
   obd_init();
   
   //in case of OBDII connection error   
   if (obd_error_flag){
     obd_err_flash();
   }  
}

//-----------------------------------------------------//
//---------------flashes fast red light----------------//
//-------in case of Bluetooth connection error---------//
//------loops for ever, need to restart Arduino--------//
void bt_err_flash(){
   
  while(1){
    digitalWrite(ledpin_red,HIGH);
    delay(100);
    digitalWrite(ledpin_red,LOW);
    delay(100);
  }
}

//-----------------------------------------------------//
//---------------flashes slow red light----------------//
//---------in case of OBDII connection error-----------//
//------loops for ever, need to restart Arduino--------//
void obd_err_flash(){
   
  while(1){
    digitalWrite(ledpin_red,HIGH);
    delay(1000);
    digitalWrite(ledpin_red,LOW);
    delay(1000);
  }
}

//-----------------------------------------------------//
//-------Just a demo-test for 7-seg disp and leds------//
void demo(){
  
  //Turn 7-seg disp OFF
  digitalWrite(led_dec,HIGH);
  digitalWrite(led_mon,HIGH);

  //Shift Light LEDS Test
  for(int i=0;i<3;i++){
    digitalWrite(ledpin_green,HIGH);
    digitalWrite(ledpin_yellow,HIGH);
    digitalWrite(ledpin_red,HIGH);
    delay(50);
    digitalWrite(ledpin_green,LOW);
    digitalWrite(ledpin_yellow,LOW);
    digitalWrite(ledpin_red,LOW);
    delay(50);
    
    }

  
  //7-seg diplays test
  for(int i=0;i<3;i++){

    for (int j=0;j<4;j++){
      sevenSegWriteDec(8);
      delay(10);
      sevenSegWriteMon(8); 
      delay(10);
    }
   //Turn 7-seg disp OFF
   digitalWrite(led_dec,HIGH);
   digitalWrite(led_mon,HIGH);
   delay(50);
  }
}

//-----------------------------------------------------//
//----------Decades 7-seg disp driver------------------//
void sevenSegWriteDec(int digit) {
    
  if (digit>0) analogWrite(led_dec,disp_bright); else digitalWrite(led_dec,HIGH); //if value is 0 stays OFF
  digitalWrite(led_mon,HIGH);                                                     //monades 7-seg disp OFF
  
  //convert dec number to 4 bits binary
  for (byte i = 0; i < 4; i++) {
    int dec_val=digit&1;                  //interested only for LSB of digit
    digitalWrite(BinaryPins[i],dec_val);  //set it to BinaryPin i=0 is LSB and goes to pin A of 74LS47 BCD-to-7-Segment
    digit=digit>>1;                       //shift right digit 1 bit 
  }
  
}

//-----------------------------------------------------//
//----------Monades 7-seg disp driver------------------//
void sevenSegWriteMon(int digit) {
  
  digitalWrite(led_dec,HIGH);              //decades 7-seg disp OFF
  analogWrite(led_mon,disp_bright);        //monades 7-seg disp ON
  
  //convert dec number to 4 bits binary
  for (byte i = 0; i < 4; i++) {
    int mon_val=digit&1;                   //interested only for LSB of digit
    digitalWrite(BinaryPins[i],mon_val);   //set it to BinaryPin i=0 is LSB and goes to pin A of 74LS47 BCD-to-7-Segment
    digit=digit>>1;                        //shift right digit 1 bit 
  }
  
}

//-----------------------------------------------------//
//----------display stored shift light RPM-------------//
//-----------------------for 3 secs--------------------//
void flash_store(int num){
  
   unsigned long start=millis();      //set start as current millis
   unsigned long dlay=3000UL;         //set delay to 3000ms
   
   while (millis()<(start+dlay))      //for 3 secs
   {
     //display num
     sevenSegWriteDec(num/10);
     delay(10);
     sevenSegWriteMon(num%10); 
     delay(10);
     //turn red led ON
     digitalWrite(ledpin_red,HIGH);
     digitalWrite(ledpin_green,LOW);
     digitalWrite(ledpin_yellow,LOW);
   } 
  
}

//-----------------------------------------------------//
//----------Retrieve RPM value from OBD----------------//
//---------convert it to readable number---------------//
void rpm_calc(){
   boolean prompt,valid;  
   char recvChar;
   char bufin[15];
   int i;

  if (!(obd_error_flag)){                                   //if no OBD connection error

     valid=false;
     prompt=false;
     blueToothSerial.print("010C1");                        //send to OBD PID command 010C is for RPM, the last 1 is for ELM to wait just for 1 respond (see ELM datasheet)
     blueToothSerial.print("\r");                           //send to OBD cariage return char
     while (blueToothSerial.available() <= 0);              //wait while no data from ELM
     i=0;
     while ((blueToothSerial.available()>0) && (!prompt)){  //if there is data from ELM and prompt is false
       recvChar = blueToothSerial.read();                   //read from ELM
       if ((i<15)&&(!(recvChar==32))) {                     //the normal respond to previus command is 010C1/r41 0C ?? ??>, so count 15 chars and ignore char 32 which is space
         bufin[i]=recvChar;                                 //put received char in bufin array
         i=i+1;                                             //increase i
       }  
       if (recvChar==62) prompt=true;                       //if received char is 62 which is '>' then prompt is true, which means that ELM response is finished 
     }

    if ((bufin[6]=='4') && (bufin[7]=='1') && (bufin[8]=='0') && (bufin[9]=='C')){ //if first four chars after our command is 410C
      valid=true;                                                                  //then we have a correct RPM response
    } else {
      valid=false;                                                                 //else we dont
    }
    if (valid){                                                                    //in case of correct RPM response
      rpm_retries=0;                                                               //reset to 0 retries
      rpm_error_flag=false;                                                        //set rpm error flag to false
      
     //start calculation of real RPM value
     //RPM is coming from OBD in two 8bit(bytes) hex numbers for example A=0B and B=6C
     //the equation is ((A * 256) + B) / 4, so 0B=11 and 6C=108
     //so rpm=((11 * 256) + 108) / 4 = 731 a normal idle car engine rpm
      rpm=0;                                                                                            
      for (i=10;i<14;i++){                              //in that 4 chars of bufin array which is the RPM value
        if ((bufin[i]>='A') && (bufin[i]<='F')){        //if char is between 'A' and 'F'
          bufin[i]-=55;                                 //'A' is int 65 minus 55 gives 10 which is int value for hex A
        } 
         
        if ((bufin[i]>='0') && (bufin[i]<='9')){        //if char is between '0' and '9'
          bufin[i]-=48;                                 //'0' is int 48 minus 48 gives 0 same as hex
        }
        
        rpm=(rpm << 4) | (bufin[i] & 0xf);              //shift left rpm 4 bits and add the 4 bits of new char
       
      }
      rpm=rpm >> 2;                                     //finaly shift right rpm 2 bits, rpm=rpm/4
    }
      
    }
    if (!valid){                                              //in case of incorrect RPM response
      rpm_error_flag=true;                                    //set rpm error flag to true
      rpm_retries+=1;                                         //add 1 retry
      rpm=0;                                                  //set rpm to 0
      //Serial.println("RPM_ERROR");
      if (rpm_retries>=RPM_CMD_RETRIES) obd_error_flag=true;  //if retries reached RPM_CMD_RETRIES limit then set obd error flag to true
    }
}

//----------------------------------------------------------//
//---------------------Send OBD Command---------------------//
//--------------------for initialitation--------------------//

void send_OBD_cmd(char *obd_cmd){
  char recvChar;
  boolean prompt;
  int retries;
 
   if (!(obd_error_flag)){                                        //if no OBD connection error
    
    prompt=false;
    retries=0;
    while((!prompt) && (retries<OBD_CMD_RETRIES)){                //while no prompt and not reached OBD cmd retries
      blueToothSerial.print(obd_cmd);                             //send OBD cmd
      blueToothSerial.print("\r");                                //send cariage return

      while (blueToothSerial.available() <= 0);                   //wait while no data from ELM
      
      while ((blueToothSerial.available()>0) && (!prompt)){       //while there is data and not prompt
        recvChar = blueToothSerial.read();                        //read from elm
        if (recvChar==62) prompt=true;                            //if received char is '>' then prompt is true
      }
      retries=retries+1;                                          //increase retries
      delay(2000);
    }
    if (retries>=OBD_CMD_RETRIES) {                               // if OBD cmd retries reached
      obd_error_flag=true;                                        // obd error flag is true
    }
  }
}
 
//----------------------------------------------------------//
//----------------initialitation of OBDII-------------------//
void obd_init(){
  
  obd_error_flag=false;     // obd error flag is false
  
  send_OBD_cmd("ATZ");      //send to OBD ATZ, reset
  delay(1000);
  send_OBD_cmd("ATSP0");    //send ATSP0, protocol auto

  send_OBD_cmd("0100");     //send 0100, retrieve available pid's 00-19
  delay(1000);
  send_OBD_cmd("0120");     //send 0120, retrieve available pid's 20-39
  delay(1000);
  send_OBD_cmd("0140");     //send 0140, retrieve available pid's 40-??
  delay(1000);
  send_OBD_cmd("010C1");    //send 010C1, RPM cmd
  delay(1000);
  
}

//----------------------------------------------------------//
//-----------start of bluetooth connection------------------//
void setupBlueToothConnection()
{
  
  bt_error_flag=false;                    //set bluetooth error flag to false
  
  enterATMode();                          //enter HC-05 AT mode
  delay(500);

  sendATCommand("RESET");                  //send to HC-05 RESET
  delay(1000);
  sendATCommand("ORGL");                   //send ORGL, reset to original properties
  sendATCommand("ROLE=1");                 //send ROLE=1, set role to master
  sendATCommand("CMODE=0");                //send CMODE=0, set connection mode to specific address
  sendATCommand("BIND=1122,33,DDEEFF");    //send BIND=??, bind HC-05 to OBD bluetooth address
  sendATCommand("INIT");                   //send INIT, cant connect without this cmd 
  delay(1000); 
  sendATCommand("PAIR=1122,33,DDEEFF,20"); //send PAIR, pair with OBD address
  delay(1000);  
  sendATCommand("LINK=1122,33,DDEEFF");    //send LINK, link with OBD address
  delay(1000); 
  enterComMode();                          //enter HC-05 comunication mode
  delay(500);
}

//----------------------------------------------------------//
//------------------reset of HC-05--------------------------//
//-------set reset pin of HC-05 to LOW for 2 secs-----------//
void resetBT()
{
 digitalWrite(Reset, LOW);
 delay(2000);
 digitalWrite(Reset, HIGH);
}

//----------------------------------------------------------//
//--------Enter HC-05 bluetooth moduel command mode---------//
//-------------set HC-05 mode pin to LOW--------------------//
void enterComMode()
{
 blueToothSerial.flush();
 delay(500);
 digitalWrite(PIO11, LOW);
 //resetBT();
 delay(500);
 blueToothSerial.begin(38400); //default communication baud rate of HC-05 is 38400
}

//----------------------------------------------------------//
//----------Enter HC-05 bluetooth moduel AT mode------------//
//-------------set HC-05 mode pin to HIGH--------------------//
void enterATMode()
{
 blueToothSerial.flush();
 delay(500);
 digitalWrite(PIO11, HIGH);
 //resetBT();
 delay(500);
 blueToothSerial.begin(38400);//HC-05 AT mode baud rate is 38400

}

//----------------------------------------------------------//

void sendATCommand(char *command)
{
  
  char recvChar;
  char str[2];
  int i,retries;
  boolean OK_flag;
  
  if (!(bt_error_flag)){                                  //if no bluetooth connection error
    retries=0;
    OK_flag=false;
    
    while ((retries<BT_CMD_RETRIES) && (!(OK_flag))){     //while not OK and bluetooth cmd retries not reached
      
       blueToothSerial.print("AT");                       //sent AT cmd to HC-05
       if(strlen(command) > 1){
         blueToothSerial.print("+");
         blueToothSerial.print(command);
       }
       blueToothSerial.print("\r\n");
      
      while (blueToothSerial.available()<=0);              //wait while no data
      
      i=0;
      while (blueToothSerial.available()>0){               // while data is available
        recvChar = blueToothSerial.read();                 //read data from HC-05
          if (i<2){
            str[i]=recvChar;                               //put received char to str
            i=i+1;
          }
      }
      retries=retries+1;                                  //increase retries 
      if ((str[0]=='O') && (str[1]=='K')) OK_flag=true;   //if response is OK then OK-flag set to true
      delay(1000);
    }
  
    if (retries>=BT_CMD_RETRIES) {                        //if bluetooth retries reached
      bt_error_flag=true;                                 //set bluetooth error flag to true
    }
  }
  
}

//----------------------------------------------------------//

void loop(){
  while (!(obd_error_flag)){            //while no OBD comunication error  
    if ((rpm>=0) && (rpm<10000)){       //if rpm value is between 0 and 10000 

      rpm_to_disp=int(rpm/100);         //devide by 100, cause we have only two 7-seg disps
      decades=rpm_to_disp / 10;         //calculate decades
      monades=rpm_to_disp % 10;         //calculate monades
     
    
      sevenSegWriteDec(decades);        //display decades to decades 7-seg disp
      delay(10);
      sevenSegWriteMon(monades);        //display monades to monades 7-seg disp
      delay(10);     
     
      if (shift_light_rpm>0){    //if shift light rpm is >0
        if (rpm_to_disp>=shift_light_rpm-10) digitalWrite(ledpin_green,HIGH); else digitalWrite(ledpin_green,LOW); //green led on -1000rpms 
        if (rpm_to_disp>=shift_light_rpm-5) digitalWrite(ledpin_yellow,HIGH); else digitalWrite(ledpin_yellow,LOW);//yellow led on -500rpms
        if (rpm_to_disp>=shift_light_rpm) digitalWrite(ledpin_red,HIGH); else digitalWrite(ledpin_red,LOW);        //red led on stored shift light rpm value
      } else digitalWrite(ledpin_green,HIGH);//if no value is stored in EEPROM
  
      if (rpm_error_flag){              //if rpm error yellow led ON
        digitalWrite(ledpin_green,LOW); 
        digitalWrite(ledpin_yellow,HIGH);
        digitalWrite(ledpin_red,LOW);
      }
      
      //if button pressed then store current rpm to EEPROM
      if ((digitalRead(sel_sw)) && (!but_pressed_flag)){
        but_pressed_flag=true;
        shift_light_rpm=rpm_to_disp;
       
        EEPROM.write(addr,shift_light_rpm);
        flash_store(shift_light_rpm); 
      } 
      if (!(digitalRead(sel_sw))) but_pressed_flag=false;
    
    }
    else //if no correct rpm value received
    {
      digitalWrite(led_dec,HIGH);
      digitalWrite(led_mon,HIGH);
      digitalWrite(ledpin_red,LOW);
      digitalWrite(ledpin_green,LOW);
      digitalWrite(ledpin_yellow,LOW);
    }
    t.update();  //update of timer for calling rpm_calc
  }
  if (obd_error_flag) obd_err_flash();    //if OBD error flag is true
}

