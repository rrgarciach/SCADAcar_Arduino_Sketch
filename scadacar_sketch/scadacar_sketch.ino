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

SoftwareSerial obd(8,7);
// Mode 1 PIDs
#define PID_ENGINE_LOAD 0104
#define PID_COOLANT_TEMP 0105
#define PID_SHORT_TERM_FUEL_TRIM_1 0106
#define PID_LONG_TERM_FUEL_TRIM_1 0107
#define PID_SHORT_TERM_FUEL_TRIM_2 0108
#define PID_LONG_TERM_FUEL_TRIM_2 0109
#define PID_FUEL_PRESSURE 010A
#define PID_INTAKE_MAP 010B
#define PID_RPM 010C
#define PID_SPEED 010D
#define PID_TIMING_ADVANCE 010E
#define PID_INTAKE_TEMP 010F
#define PID_MAF_FLOW 0110
#define PID_THROTTLE 0111
#define PID_AUX_INPUT 011E
#define PID_RUNTIME 011F
#define PID_DISTANCE_WITH_MIL 0121
#define PID_COMMANDED_EGR 012C
#define PID_EGR_ERROR 012D
#define PID_COMMANDED_EVAPORATIVE_PURGE 012E
#define PID_FUEL_LEVEL 012F
#define PID_WARMS_UPS 0130
#define PID_DISTANCE 0131
#define PID_EVAP_SYS_VAPOR_PRESSURE 0132
#define PID_BAROMETRIC 0133
#define PID_CATALYST_TEMP_B1S1 013C
#define PID_CATALYST_TEMP_B2S1 013D
#define PID_CATALYST_TEMP_B1S2 013E
#define PID_CATALYST_TEMP_B2S2 013F
#define PID_CONTROL_MODULE_VOLTAGE 0142
#define PID_ABSOLUTE_ENGINE_LOAD 0143
#define PID_RELATIVE_THROTTLE_POS 0145
#define PID_AMBIENT_TEMP 0146
#define PID_ABSOLUTE_THROTTLE_POS_B 0147
#define PID_ABSOLUTE_THROTTLE_POS_C 0148
#define PID_ACC_PEDAL_POS_D 0149
#define PID_ACC_PEDAL_POS_E 014A
#define PID_ACC_PEDAL_POS_F 014B
#define PID_COMMANDED_THROTTLE_ACTUATOR 014C
#define PID_TIME_WITH_MIL 014D
#define PID_TIME_SINCE_CODES_CLEARED 014E
#define PID_ETHANOL_FUEL 0152
#define PID_FUEL_RAIL_PRESSURE 0159
#define PID_HYBRID_BATTERY_PERCENTAGE 015B
#define PID_ENGINE_OIL_TEMP 015C
#define PID_FUEL_INJECTION_TIMING 015D
#define PID_ENGINE_FUEL_RATE 015E
#define PID_ENGINE_TORQUE_DEMANDED 0161
#define PID_ENGINE_TORQUE_PERCENTAGE 0162
#define PID_ENGINE_REF_TORQUE 0163

void setup()
{
  Serial.begin(115200);
  bt.begin(9600);
  obd.begin(9600);
  ss.begin(GPSBaud);
}

void loop()
{
  readGPS();
  
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

void sendMessage(String message)
{
  bt.flush();
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
      msg = bt.read();
      Serial.print(msg);
      delay(50);
    }
  }
}

void readOBD() {
  while (obd.available() > 0) {
    String command; //string to store entire command line
    delay(1000); // wait 1 sec to give time enough for the serial to receive all the stream
    if (obd.available()) {
      msg = obd.read();
      Serial.print(msg);
      delay(50);
    }
  }
}

void readGPS()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0) {
    delay(1000); // wait 1 sec to give time enough for the serial to receive all the stream
    if (gps.encode(ss.read())) {
      displayInfo();
    }
  }

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

void send_OBD_cmd(char *obd_cmd){
  obd.print(obd_cmd);
  obd.print("\r");
  while (obd.available() <= 0);
    while(obd.available() > 0){
      recvChar = obd.read();
  }
}
