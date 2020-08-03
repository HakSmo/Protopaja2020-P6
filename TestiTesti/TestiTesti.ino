
/* WIRING:
 *  
 *  T3.6      | DEVICE  PIN
 *  -----------------------------
 *  0 (RX1)   | CAMERA  6 (TX)
 *  1 (TX1)   | CAMERA  7 (RX)
 *  8         | DISPLAY 9 (DC)
 *  9         | DISPLAY 8 (RST)
 *  10(CS0)   | DISPLAY 7 (TFTCS)
 *  11(MOSI0) | DISPLAY 6 (MOSI)
 *  13(SCK0)  | DISPLAY 4 (SCK)
 *  
 */


#include "epc635_commands.h"
#include "crc_calc.hpp"
#include <SPI.h>
#include <ST7735_t3.h>
#include "colorLut.h"
#define CAMSERIAL Serial1
#define TFT_MISO  12
#define TFT_MOSI  11  //a12
#define TFT_SCK   13  //a13
#define TFT_DC   8 
#define TFT_CS   10  
#define TFT_RST  9
ST7735_t3 tft = ST7735_t3(TFT_CS, TFT_DC, TFT_RST);
const int ledPin = 13;
unsigned char single_frame[14] =  {SND, GET_DIST, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0xAC, 0xA8, 0xCC};
unsigned char chip_info[14] =     {SND, GET_CHIP_INFORMATION,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x94,0x8B,0x2E,0xD5};
unsigned char test_response[4] =  {0xFA, 0x03, 0x50, 0x4B};
unsigned char disable_median_filter[14] = {SND, SET_MEDIAN_FILTER, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB7, 0xE8, 0xAA, 0x4B};
unsigned char set_amplitude_limit_100[14] = {SND,SET_AMPLITUDE_LIMIT, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE7, 0x34, 0xAE, 0x47};
unsigned char set_amplitude_limit_0[14] = {SND,SET_AMPLITUDE_LIMIT, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x9B, 0xEA, 0x19};
unsigned char set_amplitude_limit_5[14] = {SND,SET_AMPLITUDE_LIMIT, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x84, 0xBB, 0x14, 0xEF};
unsigned char set_amplitude_limit_10[14] = {SND,SET_AMPLITUDE_LIMIT, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFB, 0xC7, 0xD7, 0xF0};
uint16_t target_pixel; // Used for storing displays pixel data in camera data handling.
uint16_t data_buf = 0;// Buffer for camera pixel data.

void setup() {
  
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  // initializes the display
  Serial.println("Init display");
  tft.initR(INITR_MINI160x80);  // Init ST7735S mini display
    

  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(3);
  Serial.println("Display initialized");
  
  delay(1000);
  CAMSERIAL.begin(10000000);

 /*int crcmed = calcCrc32_32(set_amplitude_limit_10, 10);
 Serial.println(crcmed, HEX);
 Serial.println("Enable median filter:");
 
 CAMSERIAL.write(set_amplitude_limit_10, 14);
   for(int i = 0; i<8; i++){
    for(;!CAMSERIAL.available(););
    if(CAMSERIAL.read() == 0x00 && i == 2){
      Serial.println("Command acknowledged");
      break;
    }else{
      Serial.println("Command not acknowledged");
      break;
    }
  }*/
  
}
// define the 16 bit distance data array NOTE!: This is no longer needed if we can just write the data straight to display buffer.
uint16_t pixelarray[160][60];

void loop() {
  /*for(unsigned int i = 0; i < 4; i++){
    digitalWrite(ledPin, HIGH);   // set the LED on
    delay(150);                  // wait for a second
    digitalWrite(ledPin, LOW);    // set the LED off
    delay(150);
  }*/
  uint8_t ret[SINGLE_PIC_SIZE];
  
  /*int a_bytes = CAMSERIAL.availableForWrite();
  
  Serial.print("Can print: ");
  Serial.print(a_bytes);
  Serial.print(" bytes\n");
  Serial.flush();*/
  CAMSERIAL.write(single_frame, 14);
  for(int i = 0; i<SINGLE_PIC_SIZE; i++){
    for(;!CAMSERIAL.available(););
    ret[i]=CAMSERIAL.read();
  }
  int crc = calcCrc32_32(ret, SINGLE_PIC_SIZE-4);
  /*for(int i = 0; i<4; i++){
    Serial.println(ret[i],HEX);
  }*/
  
 /* for(int i = 4; i< 84; i++){
    Serial.print("byteindex: ");
    Serial.print(i-4);
    Serial.print(" ");
    Serial.print(ret[i], HEX);
    Serial.print(", ");
    Serial.println(ret[i]);
  }*/

  /* Now the screen is operated using Teensy's own st7735_t3 library.
   * It is way more efficient and better optimized than adafruit's one and now
   * the screen works more smoothly.
   */
  tft.useFrameBuffer(true);   // Start the framebuffer, so we can place the camera data straight to the corresponding pixel.
  int camdata_hdr_size = 84;//pixel data starts
  data_buf = 0;// Buffer for camera pixel data.
  for(int j = 0; j<60;j++){
    for(int i = 0; i<160; i++){
      data_buf = ret[camdata_hdr_size+1];
      data_buf <<= 8;
      data_buf |= ret[camdata_hdr_size];
      data_buf <<=2;
      data_buf >>=2;
      target_pixel = tft.readPixel(i,j); // Read the target pixel being handled, save for later use.
      if(data_buf==16001){ //Case for low TOF amplitude.
        if(target_pixel != 0xFFFF){
          tft.drawPixel(i,j, 0xFFFF);
        }
      }else if(data_buf == 16002){ //Case for A/D conversion limit exceeded.
        if(target_pixel != 0xE7E0){
          tft.drawPixel(i,j, 0xE7E0);
        }
      }else if(data_buf == 16003){ //Case for pixel saturation.
        if(target_pixel != 0x5FE0){
          tft.drawPixel(i,j, 0x5FE0);
        }
      }else if(data_buf == 16007){ //Case for motion blur.
        if(target_pixel != 0xF81E){
          tft.drawPixel(i,j, 0xF81E);
        }
      }else if(data_buf == 16008){ //Case for edge detection.
        if(target_pixel != 0xFFFF){
          tft.drawPixel(i,j, 0xFFFF);
        }
      }else if(data_buf<=7500){
        if(data_buf > 2000){Serial.println(data_buf); }
        if(target_pixel != colorlut[data_buf/214]){
          tft.drawPixel(i,j, colorlut[data_buf/214]);
        }
      }else{
        tft.drawPixel(i,j, 0x0000);
      }
      camdata_hdr_size +=2 ;
    }
  }
  
  // Not tested. It basically calls the 35 member lookup table from colorLut.h
  // NOTE!: No longer needed if we can skip using pixelarray and update the camera data straight to display buffer.
  /*for(int j = 0; j<60; j++){
    for(int i = 0; i<160; i++){
      uint16_t target_pixel = tft.readPixel(i,j); // Read the pixel being handled, save for later use.
      if(pixelarray[i][j]==16008){
        if(target_pixel != 0xFFFF){
          tft.drawPixel(i,j, 0xFFFF);
        }
      }else if(pixelarray[i][j] == 16002){
        if(target_pixel != 0xFFFF){
          tft.drawPixel(i,j, 0xFFFF);
        }
      }else if(pixelarray[i][j]<=7500){
        if(target_pixel != colorlut[(int)pixelarray[i][j]/214]){
          tft.drawPixel(i,j, colorlut[(int)pixelarray[i][j]/214]);
        }
      }else{
        tft.drawPixel(i,j, 0x0000);
      }
      
    }
  }*/
  
  tft.setCursor(2, 62);
  tft.setTextColor(0xFFFF, 0x0000);
  tft.setTextWrap(true);
  tft.print("Distance: ");
  tft.print(pixelarray[80][30]);
  tft.print(" mm");
  tft.setCursor(2, 71);
  tft.print(millis()/10);
  // I don't want to print anything else to serial since it slows things down
 /* for(int i = 0; i<60;i++){
    for(int j = 0; j<160;j++){
       print(pixelarray[j][i])
    }
    println("");
  }*/
  tft.updateScreen();         //update screen
  tft.freeFrameBuffer();      //Free framebuffer
  //CRC checksum:
  Serial.println("CRC:");
  for(int i = SINGLE_PIC_SIZE-4; i<SINGLE_PIC_SIZE; i++){
    Serial.println(ret[i], HEX);
  }

  Serial.println(crc,HEX);
  Serial.print("Done!\n");

}
