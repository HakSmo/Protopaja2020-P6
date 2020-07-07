#include "epc635_commands.h"
#define CAMSERIAL Serial1
const int receivex = 0;
const int transmitx = 1;
const int ledPin = 13;
unsigned char single_frame[14] = {SND, GET_DIST, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0xAC, 0xA8, 0xCC};
unsigned char single_gs_frame[14] = {SND, GET_GS, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x72, 0x4B, 0x28, 0x68};
unsigned char test_response[4] = {0xFA, 0x03, 0x50, 0x4B};
unsigned char temp[14] = {SND, GET_TEMPERATURE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xF8, 0x6E, 0x87};
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  CAMSERIAL.begin(10000000);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  for(unsigned int i = 0; i < 4; i++){
    digitalWrite(ledPin, HIGH);   // set the LED on
    delay(150);                  // wait for a second
    digitalWrite(ledPin, LOW);    // set the LED off
    delay(150);
  }
  int ret[11] = {0};
  char ret2[14];
  int a_bytes = CAMSERIAL.availableForWrite();
  
  Serial.print("Can print: ");
  Serial.print(a_bytes);
  Serial.print(" Bytes\n");
  
  CAMSERIAL.write(temp, 14);
  

  while(!CAMSERIAL.available()){}
  a_bytes = CAMSERIAL.available();
  for(int i = 0; i<11; i++){
    ret[i] = CAMSERIAL.read();
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
  */
  for(unsigned int i = 0; i < 10; i++){
      Serial.println(ret[i],HEX);
  }
  Serial.print(a_bytes);
  Serial.print("dist done\n");
  delay(3000);
  for(;;){}
}
