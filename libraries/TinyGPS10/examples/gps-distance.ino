#include <SoftwareSerial.h>
#include <TinyGPS.h>

#define RXPIN 2
#define TXPIN 3
#define GPSBAUD 4800

TinyGPS gps;
SoftwareSerial uart_gps(RXPIN, TXPIN);

void getgps(TinyGPS &gps);

void setup(){
  Serial.begin(115200);
  uart_gps.begin(GPSBAUD);
  
  Serial.println("waiting for signal");
}

void loop(){
  
  
  //Read the serial port to see if GPS data is available
  while(uart_gps.available()){
      byte c = uart_gps.read();
     
      //If incoming data is GPS data, process it
      if(gps.encode(c)){
        getgps(gps);
      }
  }
}

void getgps(TinyGPS &gps){
  float latitude, longitude;
  
  //get and store our current lat/ long
  gps.f_get_position(&latitude, &longitude);
  
  //Calculate distance to north-pole (90º Lat  0º Long) in miles 'M'
  float northPole = distance(latitude, longitude, 90, 0, 'M');
   
  Serial.print(northPole);
  Serial.println(" Miles from the north pole");
}




float distance(float lat1, float lon1, float lat2, float lon2, char unit){ 
//Calculates the distance in between 2 points using long/lat. Set unit to 'M', 'N', or 'K' for miles, nautical miles or kilometers
  float theta = lon1 - lon2; 
  
  float dist = sin(DEG_TO_RAD * lat1) * sin(DEG_TO_RAD * lat2) +  cos(DEG_TO_RAD * lat1) * cos(DEG_TO_RAD * lat2) * cos(DEG_TO_RAD * theta); 
 
  dist = acos(dist); 
  dist = RAD_TO_DEG * dist; 
  float miles = dist * 60 * 1.1515;

  if (unit == 'K') {
    return (miles * 1.609344); 
  } else if (unit == 'N') {
    return (miles * 0.8684);
  } else {
    return miles;
  }
}