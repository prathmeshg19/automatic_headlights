#include <mbed.h>
#include <rtos.h>
#include <Thread.h>
#include "Servo.h"

UnbufferedSerial pc(USBTX,USBRX);
//inputs
AnalogIn LDR(A0);

//outputs
BusOut HeadLEDS(D8,D9,D10,D11,D12,D13);
DigitalOut warningLED(D14);
DigitalOut Buzzer(D15);
DigitalOut arduinopin(D2);
DigitalOut trig(D5);
DigitalIn echo(D6);
Servo myServo(D3);

//Interrupts
InterruptIn SOSbutton(D7);

//Threads
Thread radarthread;
Thread AHCthread;
Thread CAthread;

//timers
Timer sonar;

//public variables
int correction;
int globaldistance;
int globalangle;
int objdetected;

//method for intializing the timer and getting the software overhead timer delay
void sonarinit(){
  sonar.reset();
  sonar.start();
  while(echo == 2){};
  sonar.stop();
  correction = sonar.elapsed_time().count();
}

//metho to get object distance
int getdistance(){
  int distance;
  //int trigger = trig;
  //int echolocal = echo;
  trig = 1;
  sonar.reset();
  wait_us(10);
  trig = 0;
  while (echo == 0){};
  sonar.start();
  while (echo == 1){};
  sonar.stop();
  distance = (sonar.elapsed_time().count()-correction)*0.017;
  return distance;
  
}
//Function for radar
void radar(){
  //code for the radar will go here
  printf("Radar thread has starter");
  int maxdistance = 100;
  while(1){
  for(int i = 0; i <180; i++)
  {
    myServo = i/180;
    globaldistance = getdistance();
    globalangle = i;
    if(globaldistance < maxdistance)
    {
      objdetected = 1;
    }
  }
  for(int i = 180; i > 0; i--)
  {
    myServo = i/180;
    globaldistance = getdistance();
    globalangle = i;
    if(globaldistance < maxdistance)
    {
      objdetected = 1;
    }
  }
  }
}

void AHC(){
  printf("AHC thread has starter");
  HeadLEDS = 63;
  //int distance = 0;
  int angle = 0;
  while(1){
    //distance = globaldistance;
    angle = globalangle;
      if((objdetected = 1)){
  if(angle >= 0 && angle <= 90)
  {
    //take no action
    HeadLEDS =63;
  }
  else if(angle >=90 && angle <= 120)
  {
    HeadLEDS = 59;
  }
  else if(angle >=120 && angle <= 150)
  {
    HeadLEDS = 61;
  }
  else if(angle >= 150 && angle <= 180)
  {
    HeadLEDS = 62;
  }
  }
  }

}

void collisionalert(){
  printf("Collision alert thread has starter");
  int distance = globaldistance;
  while(1){
  distance = globaldistance;
  while (distance <30)
  {
    warningLED = 1;
    wait_us(500);
    warningLED = 0;
  }
  }
}

void SOSalert()
{
  //send location to emergency contact
  arduinopin = 1;
  //set of buzzer
  Buzzer = 1;
  while(1)
  {
    HeadLEDS = 63;
    wait_us(1000);
    HeadLEDS = 0;
  }
}

int main() {

  // put your setup code here, to run once:
  pc.baud(9600);
  sonarinit();
  radarthread.start(radar);
  CAthread.start(collisionalert);
  SOSbutton.rise(SOSalert);
  while(1)
  {
    float temp = LDR;
    int voltage = temp*10000 ;
    //printf("voltage is %d\n",voltage);
    //thread_sleep_for(1000);
    if (voltage > 200)
    {
        AHCthread.start(AHC);
    }
}
}
