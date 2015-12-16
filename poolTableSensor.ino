#include <Bridge.h>

/* Constants */

const int calibrationTime = 30;  
const int pirPin = 7;
const int bufferSize = 100;
boolean circularBuffer[bufferSize];
int pointer = 0;
int eventsInBuffer = 0;
long totalEvents = 0;
boolean active = false;

/* Calibrate PIR sensor */

void calibrateSensor() {
  Serial.print("Calibrating sensor ");
  for (int i = 0; i < calibrationTime; i++) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
}

/* Setup */

void setup() {
  Serial.begin(9600);
  calibrateSensor(); 
  Bridge.begin();
}

/* */

void incrementPointer() {
  totalEvents++;
  pointer = (pointer + 1) % bufferSize;
}

/* */ 

void logMotion(boolean motionDetected) {
  if (motionDetected && !circularBuffer[pointer]) {
    circularBuffer[pointer] = true;
    eventsInBuffer++;
  } else if (circularBuffer[pointer] && !motionDetected) {
    circularBuffer[pointer] = false;
    eventsInBuffer--;
  }
}

/* */

boolean setState() {
  int passiveToActiveThreshold = 0.4;
  int activeToPassiveThreshold = 0.2;
  double bufferRatio =  ((eventsInBuffer * 1.0) / bufferSize);
  
  if(active == false && bufferRatio >= passiveToActiveThreshold ) {
    active = true;
  } else if( active == true && bufferRatio <= activeToPassiveThreshold) {
    active = false;
  }
}

/* */ 

void loop() {
  boolean motionDetected = (digitalRead(pirPin) == HIGH);
   
  Serial.println("motionDetected:");
  Serial.println(motionDetected);
  
  logMotion(motionDetected);
  incrementPointer();
  setState();
  
  Bridge.put("motion", String(motionDetected));
  Bridge.put("eventsInBuffer", String(eventsInBuffer));
  Bridge.put("totalEvents", String(totalEvents));
  Bridge.put("active", String(active));
  
  Serial.println("Events in buffer:");
  Serial.println(eventsInBuffer);
  
  delay(500);
}
