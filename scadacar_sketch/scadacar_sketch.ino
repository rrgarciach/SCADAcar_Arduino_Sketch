  /*
 Proyect: SCADAcar
 Developed by: Ruy García
 Created: July 22, 2014
 By: Ruy García
 Javascript: parseInt( ( "41 0C 0F A0".substring(6) ).replace(" ",""),16 );
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
#define PID_ENGINE_LOAD "01041"
#define PID_COOLANT_TEMP "01051"
#define PID_SHORT_TERM_FUEL_TRIM_1 "01061"
#define PID_LONG_TERM_FUEL_TRIM_1 "01071"
#define PID_SHORT_TERM_FUEL_TRIM_2 "01081"
#define PID_LONG_TERM_FUEL_TRIM_2 "01091"
#define PID_FUEL_PRESSURE "010A1"
#define PID_INTAKE_MAP "010B1"
#define PID_RPM "010C1"
#define PID_SPEED "010D1"
#define PID_TIMING_ADVANCE "010E1"
#define PID_INTAKE_TEMP "010F1"
#define PID_MAF_FLOW "01101"
#define PID_THROTTLE "01111"
#define PID_AUX_INPUT "011E1"
#define PID_RUNTIME "011F1"
#define PID_DISTANCE_WITH_MIL "01211"
#define PID_COMMANDED_EGR "012C1"
#define PID_EGR_ERROR "012D1"
#define PID_COMMANDED_EVAPORATIVE_PURGE "012E1"
#define PID_FUEL_LEVEL "012F1"
#define PID_WARMS_UPS "01301"
#define PID_DISTANCE "01311"
#define PID_EVAP_SYS_VAPOR_PRESSURE "01321"
#define PID_BAROMETRIC "01331"
#define PID_CATALYST_TEMP_B1S1 "013C1"
#define PID_CATALYST_TEMP_B2S1 "013D1"
#define PID_CATALYST_TEMP_B1S2 "013E1"
#define PID_CATALYST_TEMP_B2S2 "013F1"
#define PID_CONTROL_MODULE_VOLTAGE "01421"
#define PID_ABSOLUTE_ENGINE_LOAD "01431"
#define PID_RELATIVE_THROTTLE_POS "01451"
#define PID_AMBIENT_TEMP "01461"
#define PID_ABSOLUTE_THROTTLE_POS_B "01471"
#define PID_ABSOLUTE_THROTTLE_POS_C "01481"
#define PID_ACC_PEDAL_POS_D "01491"
#define PID_ACC_PEDAL_POS_E "014A1"
#define PID_ACC_PEDAL_POS_F "014B1"
#define PID_COMMANDED_THROTTLE_ACTUATOR "014C1"
#define PID_TIME_WITH_MIL "014D1"
#define PID_TIME_SINCE_CODES_CLEARED "014E1"
#define PID_ETHANOL_FUEL "01521"
#define PID_FUEL_RAIL_PRESSURE "01591"
#define PID_HYBRID_BATTERY_PERCENTAGE "015B1"
#define PID_ENGINE_OIL_TEMP" 015C1"
#define PID_FUEL_INJECTION_TIMING "015D1"
#define PID_ENGINE_FUEL_RATE "015E1"
#define PID_ENGINE_TORQUE_DEMANDED "01611"
#define PID_ENGINE_TORQUE_PERCENTAGE "01621"
#define PID_ENGINE_REF_TORQUE "01631"

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
  /*
  send_OBD_cmd("ATZ");      //send to OBD ATZ, reset
  delay(1000);
  send_OBD_cmd("ATSP0");    //send ATSP0, protocol auto
  send_OBD_cmd("0100");     //send 0100, retrieve available pid's 00-19
  delay(1000);
  send_OBD_cmd("0120");     //send 0120, retrieve available pid's 20-39
  delay(1000);
  send_OBD_cmd("0140");     //send 0140, retrieve available pid's 40-??
  delay(1000);
  send_OBD_cmd(PID_RPM);    //send 010C1, RPM cmd
  */
  delay(1000);
  readMaster();
  delay(1000);
}

void sendMessage(String message)
{
  bt.flush();
  // Serial.print("Sending message: ");
  //Serial.println(message);
  bt.println(message);
}

void readMaster()
{
  if (bt.available() > 0) {
    String msg; // string to store entire command line
    while (bt.available()) {
      msg += bt.read();
    }
    Serial.println(msg);
    delay(50);
    checkIncommingCmd( msg.toInt() );
  }
  bt.flush();
}

void readOBD()
{
  if (obd.available() > 0) {
    String msg; // string to store entire command line
    delay(1000); // wait 1 sec to give time enough for the serial to receive all the stream
    while (obd.available()) {
      msg += obd.read();
    }
    Serial.println(msg);
    delay(50);
  } else {
    sendMessage("No data from OBD2.");
  }
  obd.flush();
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
  ss.flush();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    // while(true);
  }
}

void displayInfo()
{
  String strValue;
  char charBuf[15];
  if ( gps.location.isValid() ) {
    // This function is thanks to http://dereenigne.org/arduino/arduino-float-to-string
    dtostrf(gps.location.lat(), 12, 6, charBuf);
    strValue += charBuf;
    // Serial.print(strValue);
    strValue += ",";
    // Serial.print(F(","));
    dtostrf(gps.location.lng(), 12, 6, charBuf);
    strValue += charBuf;
    // Serial.print(charBuf);
  }
  else {
    // Serial.print(F("INVALID"));
    strValue += "INVALID";
  }

  // Serial.println();
  // This will execute the sending message function when we want suposing that
  // each second it reads the TinyGPS
  if(readCount++ % delayTime == 0) {
    sendMessage(strValue);
  }
}

void send_OBD_cmd(char *obd_cmd)
{
  obd.println(obd_cmd);
  readOBD();
}

void checkIncommingCmd(int cmd)
{
  switch (cmd) {
    case 0:
      send_OBD_cmd(PID_RPM);
      break;
    case 1:
      send_OBD_cmd(PID_SPEED);
      break;
    case 2:
      send_OBD_cmd(PID_FUEL_LEVEL);
      break;
    case 3:
      send_OBD_cmd(PID_DISTANCE);
      break;
    case 4:
      send_OBD_cmd(PID_THROTTLE);
      break;
  }
}
