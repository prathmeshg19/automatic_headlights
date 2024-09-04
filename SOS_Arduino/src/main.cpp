#include <Arduino.h>
#include "SoftwareSerial.h"
#include "TinyGPSPlus.h"

const int gpsRX = 6;
const int gpsTX = 7;
const int gsmRX = 0;
const int gsmTX = 1;
const int trigger = 8;
// put function declarations here:
TinyGPSPlus gps;
SoftwareSerial gpsserial(gpsRX,gpsTX);
SoftwareSerial gsm(gsmRX,gsmTX);
double latitude;
double longitude;

void gpsinit(){
  gpsserial.begin(9600);
  Serial.begin(9600);
}

void getlocation(){
  if(gps.location.isValid())
  {
    latitude = gps.location.lat();
    longitude = gps.location.lng();
  }
  else{
    Serial.println("GPS Not Available");
  }
}
void gsminit(){
  gsm.begin(9600);
  gsm.println("AT");
}

void gsmsend(){
  gsm.println("AT+CMFG=1");
  gsm.println("AT+CMGS=\"+91XXXXXXXXXX\"");
  gsm.println("HI,Your car seems to have had an accident the location for the car is \n Latitude:");
  gsm.println(latitude);
  gsm.println("\n");
  gsm.println("Longitude:");
  gsm.println(longitude);
}
void setup() {
  // put your setup code here, to run once:
  gpsinit();
  gsminit();
  pinMode(trigger,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  while(1){
  getlocation();
  int soswitch = digitalRead(trigger);
  if(soswitch = 1)
  {
    gsmsend();
  }
  }
}
