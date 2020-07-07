#include "epc635_commands.h"
#include "crc_calc.hpp"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define CAMSERIAL Serial1
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


const int receivex = 0;
const int transmitx = 1;
const int ledPin = 13;
unsigned char single_frame[14] = {SND, GET_DIST, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0xAC, 0xA8, 0xCC};
unsigned char chip_info[14] = {SND, GET_CHIP_INFORMATION,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x94,0x8B,0x2E,0xD5};
unsigned char test_response[4] = {0xFA, 0x03, 0x50, 0x4B};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  CAMSERIAL.begin(10000000);
  pinMode(ledPin, OUTPUT);
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(128, 63, SSD1306_WHITE);
  display.display();
}
uint16_t pixelarray[160][60];
void loop() {
  /*for(unsigned int i = 0; i < 4; i++){
    digitalWrite(ledPin, HIGH);   // set the LED on
    delay(150);                  // wait for a second
    digitalWrite(ledPin, LOW);    // set the LED off
    delay(150);
  }*/
  uint8_t ret[SINGLE_PIC_SIZE];
  int a_bytes = CAMSERIAL.availableForWrite();
  
  Serial.print("Can print: ");
  Serial.print(a_bytes);
  Serial.print(" Bytes\n");
  CAMSERIAL.write(single_frame, 14);
  for(int i = 0; i<SINGLE_PIC_SIZE; i++){
    for(;!CAMSERIAL.available(););
    ret[i]=CAMSERIAL.read();
  }
  
  int crc = calcCrc32_32(ret, SINGLE_PIC_SIZE-4);
  for(int i = 0; i<4; i++){
    Serial.println(ret[i],HEX);
  }
  /*for(int i = 4; i< 84; i++){
    Serial.print("byteindex: ");
    Serial.print(i-4);
    Serial.print(" ");
    Serial.print(ret[i], HEX);
    Serial.print(", ");
    Serial.println(ret[i]);
  }*/
  int k = 84;//pixel data starts
  uint16_t buf = 0;//buffer
  for(int j = 0; j<60;j++){
    for(int i = 0; i<160; i++){
      buf = ret[k+1];
      buf <<= 8;
      buf |= ret[k];
      buf <<=2;
      buf >>=2;
      pixelarray[i][j] = buf;
      k +=2 ;
    }
  }
  for(int j = 0; j<SCREEN_HEIGHT-4; j++){
    for(int i = 0; i<SCREEN_WIDTH; i++){
      if(pixelarray[i][j]< 7500){
        display.drawPixel(i, j, SSD1306_WHITE);
      }else{
        display.drawPixel(i,j, SSD1306_BLACK);
      }
    }
  }
  display.display();
  
 /* for(int i = 0; i<60;i++){
    for(int j = 0; j<160;j++){
       print(pixelarray[j][i])
    }
    println("");
  }*/
  //CRC checksum:
  /*Serial.println("CRC:");
  for(int i = SINGLE_PIC_SIZE-4; i<SINGLE_PIC_SIZE; i++){
    Serial.println(ret[i], HEX);
  }*/

  //Serial.println(crc,HEX);
  //Serial.print("Done!\n");


}
