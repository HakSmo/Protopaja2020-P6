#include "epc635_commands.h"
#define CAMSERIAL Serial1
const int receivex = 0;
const int transmitx = 1;
const int ledPin = 13;
unsigned char single_frame[14] = {SND, GET_DIST, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0xAC, 0xA8, 0xCC};
unsigned char test_response[4] = {0xFA, 0x03, 0x50, 0x4B};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  CamSerial.begin(10000000);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  for(unsigned int i = 0; i < 4; i++){
    digitalWrite(ledPin, HIGH);   // set the LED on
    delay(150);                  // wait for a second
    digitalWrite(ledPin, LOW);    // set the LED off
    delay(150);
  }
  char ret[14];
  int a_bytes = CamSerial.availableForWrite();
  
  Serial.print("Can print: ");
  Serial.print(a_bytes);
  Serial.print(" Bytes\n");
  CAMSERIAL.write(single_frame, 14);
  if(CAMSERIAL.available() == 0){
    Serial.print("")
  }
  /*CamSerial.write(single_frame,14);
  for(unsigned int i = 0; i < 14; i++){
    if(CamSerial.available() > 0){
      ret[i] = CamSerial.read(); 
    }
    else{
    Serial.print("Camera not found!\n");
    break;
    }
  }
  for(unsigned int i = 0; i < 14; i++){
    if(ret[i] != test_response[i]){
      Serial.print("Values do not match!\nExpected:");
      Serial.print(test_response[i],HEX);
      Serial.print(", got: ");
      Serial.print(ret[i],HEX);
      Serial.print("\n");
    }
  }*/
  Serial.print("Done!\n");
  delay(3000);
}
